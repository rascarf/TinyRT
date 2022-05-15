#include <iostream>
#include <time.h>
#include <math.h>

#include "Core/Scene.h"

#include "SDL2/SDL.h"
#include "GL/gl3w.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "ImGuizmo.h"
#include "tinydir.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
using namespace std;

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
}

void GetEnvMaps()
{
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
}


//TODO
void LoadScene(std::string sceneName)
{
    printf("Load %s\n", sceneName.c_str());
}

void DrawUI()
{
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


void MainLoop(void* arg)
{
    LoopData& loopdata = *(LoopData*)arg;
    SDL_Event event;

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
                printf("Resize!");
            }

            if (event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(loopdata.mWindow))
            {
                done = true;
            }
        }
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(loopdata.mWindow);
    ImGui::NewFrame();
    ImGuizmo::SetOrthographic(false);

    DrawUI();

    double presentTime = SDL_GetTicks();
    // Update((float)(presentTime - lastTime));

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

    std::string sceneFile;


    GetSceneFiles();
    GetEnvMaps();
    // LoadScene(sceneFiles[sampleSceneIdx]);



    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error:%s \n", SDL_GetError());
    }

    LoopData loopdata;

    const char* glsl_version = "#version 130";
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
    loopdata.mWindow = SDL_CreateWindow("GLSL PathTracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 1024, window_flags);
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

    loopdata.mGLContext = SDL_GL_CreateContext(loopdata.mWindow);
    if (!loopdata.mGLContext)
    {
        fprintf(stderr, "Failed to initialize GL context!\n");
        return 1;
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
        return 1;
    }
#endif

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplSDL2_InitForOpenGL(loopdata.mWindow, loopdata.mGLContext);

    ImGui_ImplOpenGL3_Init(glsl_version);

    ImGui::StyleColorsDark();

    while (!done)
    {
        MainLoop(&loopdata);
    }

    // delete renderer;
    // delete scene;

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(loopdata.mGLContext);
    SDL_DestroyWindow(loopdata.mWindow);
    SDL_Quit();
    return 0;

}