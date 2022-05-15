#include <iostream>
#include <time.h>
#include <math.h>
#include <optional>

#include "Core/Scene.h"
#include "Core/Camera.h"
#include "Loaders/Loader.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include "SDL2/SDL.h"
#include "GL/gl3w.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "ImGuizmo.h"
#include "tinydir.h"
#include "Loaders/GLTFLoader.h"

using namespace std;

Scene* scene = nullptr;
// Renderer* renderer = nullptr; TODO

std::string shadersDir = "../src/shaders/";
std::string assetsDir = "./assets/";
std::string envMapDir = "./assets/HDR/";

std::vector<std::string> sceneFiles;
std::vector<std::string> envMaps;

float mouseSensitivity = 0.01f;
bool keyPressed = false;

int sampleSceneIdx = 0;
int selectedInstance = 0;

double lastTime = SDL_GetTicks();
int envMapIdx = 0;
bool done = false;

RenderOptions renderOptions;

struct LoopData
{
    SDL_Window* mWindow = nullptr;
    SDL_GLContext mGLContext = nullptr;
};

void GetSceneFiles()
{
    printf("Collect Scenes.\n");
    tinydir_dir dir;
    int i;
    tinydir_open_sorted(&dir, assetsDir.c_str());

    for (i = 0; i < dir.n_files; i++)
    {
        tinydir_file file;
        tinydir_readfile_n(&dir, &file, i);

        std::string ext = std::string(file.extension);
        if (ext == "scene" || ext == "gltf" || ext == "glb")
        {
            sceneFiles.push_back(assetsDir + std::string(file.name));
        }
    }

    tinydir_close(&dir);
    printf("Collect Scenes Done!\n");
}
void GetEnvMaps()
{
    printf("Collect EnvMaps.\n");
    tinydir_dir dir;
    int i;
    tinydir_open_sorted(&dir, envMapDir.c_str());

    for (i = 0; i < dir.n_files; i++)
    {
        tinydir_file file;
        tinydir_readfile_n(&dir, &file, i);

        std::string ext = std::string(file.extension);
        if (ext == "hdr")
        {
            envMaps.push_back(envMapDir + std::string(file.name));
        }
    }

    tinydir_close(&dir);
    printf("Collect EnvMaps Done!\n");
}

void LoadScene(std::string sceneName)
{
    printf("Load %s\n", sceneName.c_str());

    if (scene)
        delete scene;

    scene = new Scene();

    std::string ext = sceneName.substr(sceneName.find_last_of(".") + 1);
    bool success = false;
    Mat4 xform;

    if (ext == "scene")
        success = LoadSceneFromFile(sceneName, scene, renderOptions);
    else if (ext == "gltf")
        success = LoadGLTF(sceneName, scene, renderOptions, xform, false);
    else if (ext == "glb")
        success = LoadGLTF(sceneName, scene, renderOptions, xform, true);

    if (!success)
    {
        printf("Unable to load scene\n");
        exit(0);
    }

    selectedInstance = 0;

    // Add a default HDR if there are no lights in the scene
    if (!scene->envMap && !envMaps.empty())
    {
        scene->AddEnvMap(envMaps[envMapIdx]);
        renderOptions.enableEnvMap = scene->lights.empty() ? true : false;
        renderOptions.envMapIntensity = 1.5f;
    }

    scene->renderOptions = renderOptions;
}

void InitUI(const char* glsl_version, const LoopData& loopdata)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplSDL2_InitForOpenGL(loopdata.mWindow, loopdata.mGLContext);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();
}
void DrawUI(const LoopData& loopdata)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(loopdata.mWindow);
    ImGui::NewFrame();
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::BeginFrame();
    {
        ImGui::Begin("Settings");

        // Scenes
        std::vector<const char*> scenes;
        for (int i = 0; i < sceneFiles.size(); ++i)
            scenes.push_back(sceneFiles[i].c_str());
        if (ImGui::Combo("Scene", &sampleSceneIdx, scenes.data(), scenes.size()))
        {
            printf("Click Scene\n");
        }

        // Environment maps
        std::vector<const char*> envMapsList;
        for (int i = 0; i < envMaps.size(); ++i)
            envMapsList.push_back(envMaps[i].c_str());
        if (ImGui::Combo("EnvMaps", &envMapIdx, envMapsList.data(), envMapsList.size()))
        {
            printf("Click EnvMaps\n");
        }

    }
    ImGui::End();
}
void DestoryUI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

std::optional<LoopData> InitSDLAndOpenGL()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error:%s \n", SDL_GetError());
    }

    LoopData loopdata;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

    loopdata.mWindow = SDL_CreateWindow("PathTracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, renderOptions.windowResolution.x, renderOptions.windowResolution.y, window_flags);
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

    loopdata.mGLContext = SDL_GL_CreateContext(loopdata.mWindow);

    if (!loopdata.mGLContext)
    {
        fprintf(stderr, "Failed to initialize GL context!\n");
        return nullopt;
    }

    SDL_GL_SetSwapInterval(0); // Disable vsync

#if GL_VERSION_3_2
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return nullopt;
    }
#endif

    return loopdata;
}
void DestorySDLAndOpenGL(LoopData& loopdata)
{
    SDL_GL_DeleteContext(loopdata.mGLContext);
    SDL_DestroyWindow(loopdata.mWindow);
    SDL_Quit();
}

void WindowEvent(LoopData& loopdata)
{
    SDL_Event event;

    //Window Event
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
        {
            done = true;
        }

        if (event.type == SDL_WINDOWEVENT)
        {
            if (event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                printf("Resize!\n");
                renderOptions.windowResolution = iVec2(event.window.data1, event.window.data2);

                if (!renderOptions.independentRenderSize)
                    renderOptions.renderResolution = renderOptions.windowResolution;

                scene->renderOptions = renderOptions;
                // renderer->ResizeRenderer(); TODO
            }

            if (event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(loopdata.mWindow))
            {
                done = true;
            }
        }
    }
}

void Update(float secondElapsed)
{
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow) && ImGui::IsAnyMouseDown() && !ImGuizmo::IsOver())
    {
        //TODO
        if (ImGui::IsMouseDown(0))
        {
            printf("Mouse Down 0 \n");
            ImVec2 mouseDelta = ImGui::GetMouseDragDelta(0, 0);
            // scene->camera->OffsetOrientation(mouseDelta.x, mouseDelta.y);
            ImGui::ResetMouseDragDelta(0);
        }
        else if (ImGui::IsMouseDown(1))
        {
            printf("Mouse Down 1\n");
            ImVec2 mouseDelta = ImGui::GetMouseDragDelta(1, 0);
            // scene->camera->SetRadius(mouseSensitivity * mouseDelta.y);
            ImGui::ResetMouseDragDelta(1);
        }
        else if (ImGui::IsMouseDown(2))
        {
            printf("Mouse Down 2\n");
            ImVec2 mouseDelta = ImGui::GetMouseDragDelta(2, 0);
            // scene->camera->Strafe(mouseSensitivity * mouseDelta.x, mouseSensitivity * mouseDelta.y);
            ImGui::ResetMouseDragDelta(2);
        }
        scene->dirty = true;
    }

    //renderer->Update(secondsElapsed); TODO

}

void MainLoop(void* arg)
{
    LoopData& loopdata = *(LoopData*)arg;
    WindowEvent(loopdata);

    DrawUI(loopdata);

    double presentTime = SDL_GetTicks();
    Update((float)(presentTime - lastTime));
    lastTime = presentTime;

    glClearColor(0.5, 0.5, 1., 1.);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    // Render();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 1024, 1024);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(loopdata.mWindow);
}


int main(int argc, char* argv[])
{
    srand((unsigned int)time(0));

    //Gather Scenes and EnvironmentMap
    GetSceneFiles();
    GetEnvMaps();

    //Load Default Scene
    if(!sceneFiles.empty())
    {
        LoadScene(sceneFiles[sampleSceneIdx]);
    }
    else
    {
        printf("Error:sceneFiles is empty");
    }

    //Create SDL and OpenGL
    LoopData loopdata;
    std::optional<LoopData> OptionLoopData= InitSDLAndOpenGL();
    if(OptionLoopData.has_value())
    {
        loopdata = OptionLoopData.value();
    }
    else
    {
        printf("Error:can not initialize SDL");
        return -1;
    }

    // Create Dear ImGui context
    const char* glsl_version = "#version 130";
    InitUI(glsl_version, loopdata);

    //Loop
    while (!done)
    {
        MainLoop(&loopdata);
    }

    // delete renderer; TODO
    delete scene;

    DestorySDLAndOpenGL(loopdata);

    return 0;
}