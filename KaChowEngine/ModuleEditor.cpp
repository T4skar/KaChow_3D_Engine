#include "ModuleEditor.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "OurPrimitive.h"
#include "GameObject.h"

#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_opengl3.h"
#include "ImGui/backends/imgui_impl_sdl2.h"

ModuleEditor::ModuleEditor(Application * app, bool start_enabled) : Module(app, start_enabled)
{
    logVector = new std::vector<std::string>();
    name = "Editor";
}

ModuleEditor::~ModuleEditor()
{

}

bool ModuleEditor::Init()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls


    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    // "pilota"
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    // Canviar color de editor
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
    ImGui_ImplOpenGL3_Init();

    mFPSLog.reserve(30);
    mMsLog.reserve(30);

    gameWindowSize = { 0,0 };
    sceneWindowSize = { 0,0 };

    cvCounter = 0;

    // Config window
    fullscreen = false;
    screenWidth = SCREEN_WIDTH;

    // Config opengl vars init
   depthTest = false;
   isCullFace = false;
   lightning = false;
   colorMat = false;
   texture2D = false;
   vSync = true;
   isWireframe = false;
   bright = 0.0f;
   bright_aux = 0.0f;

   isAABB = true;

   isActivatedHierarchy = true;
   isActivatedDemo = false;
   isActivatedInspector = true;
   isActivatedConsole = true;
   isActivatedGameWindow = true;
   isActivatedSceneWindow = true;

   GetHardwareInfo();

   //playButton = App->texture2D->LoadTexture("Assets/UI/play.png");
   //stopButton = App->texture2D->LoadTexture("Assets/UI/stop.png");
   //pauseButton = App->texture2D->LoadTexture("Assets/UI/pause.png");

    return true;
}


// Tot lo de ImGui ha de passar per aqui
update_status ModuleEditor::DrawEditor()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();

    // OpenGL Stuff

    // ImGui Stuff

    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
    ImGuizmo::Enable(true);

    AddHistogramData(App->GetDT() * 1000.0f, mMsLog);
    AddHistogramData(1 / App->GetDT(), mFPSLog);

    // Docking for all windows
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNavFocus;

    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(main_viewport->WorkPos);
    ImGui::SetNextWindowSize(main_viewport->Size);
    ImGui::SetNextWindowViewport(main_viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin(" ", nullptr, flags);

    ImGui::PopStyleVar(3);

    ImGuiID dockID = ImGui::GetID("MainDockZone");
    ImGui::DockSpace(dockID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::End();
    // Finish docking for all windows

    // For Quit button on file
    update_status ret = update_status::UPDATE_CONTINUE;
    // HERE ALL WINDOW RENDERS
    // Always EndMenu when Begin Menu
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
            {
                App->scene->SaveSceneRequest();
            }

            if (ImGui::MenuItem("Load Scene"))
            {
                App->scene->LoadSceneRequest();
            }

            if (ImGui::MenuItem("Quit", "ESC"))
                ret = UPDATE_STOP;

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Assets"))
        {
            ImGui::Text("Hello world!");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Objects"))
        {
            if (ImGui::MenuItem("Empty Object")) {
                OurPrimitive::CreatePrimitive(ShapeType::EMPTY);
            }

            if (ImGui::MenuItem("Plane")) {
                OurPrimitive::CreatePrimitive(ShapeType::PLANE);
            }

            if (ImGui::MenuItem("Cube")) {
                OurPrimitive::CreatePrimitive(ShapeType::CUBE);
            }

            if (ImGui::MenuItem("Sphere")) {
                OurPrimitive::CreatePrimitive(ShapeType::SPHERE);
            }

            if (ImGui::MenuItem("Cylinder")) {
                OurPrimitive::CreatePrimitive(ShapeType::CYLINDER);
            }

            if (ImGui::MenuItem("Monkey")) {
                OurPrimitive::CreatePrimitive(ShapeType::MONKEY);
            }

            if (ImGui::MenuItem("Icosphere")) {
                OurPrimitive::CreatePrimitive(ShapeType::ICOSHPERE);
            }

            if (ImGui::MenuItem("Cone")) {
                OurPrimitive::CreatePrimitive(ShapeType::CONE);
            }

            if (ImGui::MenuItem("Torus")) {
                OurPrimitive::CreatePrimitive(ShapeType::TORUS);
            }


            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Configuration"))
        {
            ImGuiIO& io = ImGui::GetIO();

            static int volumeLevel = 0;

            if (ImGui::Button("Save"))App->SaveConfigRequest();
            ImGui::SameLine();
            if (ImGui::Button("Load"))App->LoadConfigRequest();

            if (ImGui::CollapsingHeader("FPS Histogram"))
            {
                ImGui::PlotHistogram("FPS", &mFPSLog[0], mFPSLog.size(), 0, "FPS", 0.0f, 100.0f, ImVec2(450, 100));
                ImGui::PlotHistogram("Ms.", &mMsLog[0], mMsLog.size(), 0, "Miliseconds", 0.0f, 100.0f, ImVec2(450, 100));
            }


            // Window
            if (ImGui::CollapsingHeader("Window"))
            {
                ImGui::Text("Window Size: %d x %d", App->window->e_width, App->window->e_height);
                if (ImGui::Checkbox("Fullscreen", &fullscreen))
                {
                    if (fullscreen)
                    {
                        App->window->SetFullscreen();
                    }
                    else
                    {
                        App->window->SetResizable();
                    }
                }

                /*int preWidth = App->window->e_width;
                int preHeight = App->window->e_height;

                ImGui::SliderInt("Window Width", &App->window->e_width, 320, 2560);
                if (preWidth != App->window->e_width)
                {
                    App->window->ChangeWidth();
                }

                ImGui::SliderInt("Window Height", &App->window->e_height, 256, 2048);
                if (preHeight != App->window->e_height)
                {
                    App->window->ChangeHeight();
                }*/


                ImGui::Text("View");
                if (ImGui::Checkbox("DemoWindow", &isActivatedDemo))
                {
                    isActivatedDemo != isActivatedDemo;
                }
                if (ImGui::Checkbox("Hierarchy", &isActivatedHierarchy))
                {
                    isActivatedHierarchy != isActivatedHierarchy;
                }
                if (ImGui::Checkbox("Inspector", &isActivatedInspector))
                {
                    isActivatedInspector != isActivatedInspector;
                }
                if (ImGui::Checkbox("Console", &isActivatedConsole))
                {
                    isActivatedConsole != isActivatedConsole;
                }
                if (ImGui::Checkbox("Scene Window", &isActivatedSceneWindow))
                {
                    isActivatedSceneWindow != isActivatedSceneWindow;
                }
                if (ImGui::Checkbox("Game Window", &isActivatedGameWindow))
                {
                    isActivatedGameWindow != isActivatedGameWindow;
                }
            }

            if (ImGui::CollapsingHeader("Camera"))
            {
                const char* listType[]{ "Perspective", "Orthographic" };
                ImGui::Text("Camera type:\t ");
                ImGui::SameLine();
                if (ImGui::Combo("CameraType", &App->camera->typeCameraSelected, listType, IM_ARRAYSIZE(listType)))
                {
                    if (App->camera->typeCameraSelected == 0)
                        App->camera->camera->frustum.type = PerspectiveFrustum;

                    if (App->camera->typeCameraSelected == 1)
                        App->camera->camera->frustum.type = OrthographicFrustum;
                }
                if (ImGui::SliderInt("FOV", &App->camera->camera->FOV, 5, 200)) {
                    App->camera->camera->frustum.verticalFov = App->camera->camera->FOV * DEGTORAD;
                    App->camera->camera->frustum.horizontalFov = 2.0f * atanf(tanf(App->camera->camera->frustum.verticalFov / 2.0f) * 1.7f);
                }
                if (ImGui::Button("Reset FOV")) {
                    App->camera->camera->FOV = 60.0f;

                    App->camera->camera->frustum.verticalFov = App->camera->camera->FOV * DEGTORAD;
                    App->camera->camera->frustum.horizontalFov = 2.0f * atanf(tanf(App->camera->camera->frustum.verticalFov / 2.0f) * 1.7f);
                }


                ImGui::Text("Near Distance\t");
                ImGui::SameLine();
                if (ImGui::SliderFloat("NearDistance", &App->camera->camera->frustum.nearPlaneDistance, 0.1f, App->camera->camera->frustum.farPlaneDistance))
                {
                    App->camera->camera->frustum.nearPlaneDistance = App->camera->camera->frustum.nearPlaneDistance;
                }
                if (ImGui::Button("Reset Near Distance")) {
                    App->camera->camera->frustum.nearPlaneDistance = 0.1f;
                }

                ImGui::Text("Far Distance\t ");
                ImGui::SameLine();
                if (ImGui::SliderFloat("FarDistance", &App->camera->camera->frustum.farPlaneDistance, 0.1f, 1000.f))
                {
                    if (App->camera->camera->frustum.farPlaneDistance <= App->camera->camera->frustum.nearPlaneDistance)
                    {
                        App->camera->camera->frustum.nearPlaneDistance = App->camera->camera->frustum.farPlaneDistance;
                    }

                    App->camera->camera->frustum.farPlaneDistance = App->camera->camera->frustum.farPlaneDistance;
                }
                if (ImGui::Button("Reset Far Distance")) {
                    App->camera->camera->frustum.farPlaneDistance = 500.f;
                }
            }

            // Render
            if (ImGui::CollapsingHeader("Render"))
            {
                //ImGui::Checkbox("vSync", vsyncBoolTest);
                if (ImGui::TreeNode("OpenGL"))
                {

                    if (ImGui::Checkbox("Lightning", &lightning))
                    {
                        if (lightning)
                        {
                            glEnable(GL_LIGHTING);
                            LOG("Lights On");
                        }
                        else
                        {
                            glDisable(GL_LIGHTING);
                            LOG("Lights Off");
                        }
                    }

                    if (ImGui::Checkbox("Cull Face", &isCullFace))
                    {
                        if (isCullFace)
                        {
                            glEnable(GL_CULL_FACE);
                            LOG("Cull Face On");
                        }
                        else
                        {
                            glDisable(GL_CULL_FACE);
                            LOG("Cull Face Off");
                        }
                    }

                    if (ImGui::Checkbox("Depth Test", &depthTest))
                    {
                        if (depthTest)
                        {
                            glEnable(GL_DEPTH_TEST);
                            LOG("Depth Test On");
                        }
                        else
                        {
                            glDisable(GL_DEPTH_TEST);
                            LOG("Depth Test Off");
                        }
                    }

                    if (ImGui::Checkbox("Color Material", &colorMat))
                    {
                        if (colorMat)
                        {
                            glEnable(GL_COLOR_MATERIAL);
                            LOG("Color Material On");
                        }
                        else
                        {
                            glDisable(GL_COLOR_MATERIAL);
                            LOG("Color Material Off");
                        }
                    }

                    //Wireframe option
                    if (ImGui::Checkbox("Wireframe", &isWireframe))
                    {
                        if (isWireframe)
                        {
                            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                            LOG("Wireframe Mode On");
                        }
                        else
                        {
                            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                            LOG("Wireframe Mode Off");
                        }
                    }

                    if (ImGui::Checkbox("Texture 2D", &texture2D))
                    {
                        if (texture2D)
                        {
                            glEnable(GL_TEXTURE_2D);
                            LOG("Texture 2D On");
                        }
                        else
                        {
                            glDisable(GL_TEXTURE_2D);
                            LOG("Texture 2D Off");
                        }
                    }

                    
                    ImGui::Text("Brightness");
                    ImGui::Text("Min");
                    ImGui::SameLine();
                    if (ImGui::SliderFloat(" Max", &bright, 0.100f, 1.000f))
                    {
                        SDL_SetWindowBrightness(App->window->window, bright);
                        bright_aux = bright * 100;
                    }

                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Debug"))
                {

                    if (ImGui::Checkbox("Draw AABB", &isAABB))
                    {
                        if (isAABB)
                        {
                            LOG("AABB On");
                        }
                        else
                        {
                            LOG("AABB Off");
                        }
                    }

                    ImGui::TreePop();
                }

            }

            // Input
            if (ImGui::CollapsingHeader("Input"))
            {
                // MOUSE
                ImGui::Text("Mouse");
                if (ImGui::IsMousePosValid())
                    ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
                else
                    ImGui::Text("Mouse pos: <INVALID>");
                ImGui::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);
                ImGui::Text("Mouse down:");
                for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseDown(i)) { ImGui::SameLine(); ImGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
                ImGui::Text("Mouse wheel: %.1f", io.MouseWheel);
                // KEYBOARD
                ImGui::Text("Keyboard");
            }

            if (ImGui::CollapsingHeader("Audio"))
            {
                // Audio
                ImGui::SliderInt("Volume", &volumeLevel, 0, 100);
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Hardware Info"))
        {
            ImGui::Text("GLSL: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
            ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
            ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));

            ImGui::Separator();
            ImGui::Text("CPUs: %d (Cache: %dkb)", CPUCount, CPUCache);
            ImGui::Text("System RAM: %.1fGb", systemRAM);
            ImGui::Text("Caps: %s", caps.c_str());

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("About"))
        {
            ImGuiIO& io = ImGui::GetIO();

            ImGui::SeparatorText("ABOUT THIS ENGINE:");
            ImGui::Text("KaChow 3D Engine");
            ImGui::Text("I'm faster than fast. Faster than fast. I'm lightning!");
            ImGui::NewLine();

            ImGui::Text("By Albert Martin & Adrian Ramirez");
            if (ImGui::Button("Github Repository"))
            {
                ShellExecute(0, 0, "https://github.com/T4skar/KaChow_3D_Engine", 0, 0, SW_SHOW);
            }

            ImGui::SeparatorText("3rd Party Libraries used");

            if (ImGui::Button("SDL"))
            {
                ShellExecute(0, 0, "https://libsdl.org/index.php", 0, 0, SW_SHOW);
            }
            ImGui::SameLine();
            ImGui::Text("Version: %s", SDLVersion.c_str());
            if (ImGui::Button("OpenGL"))
            {
                ShellExecute(0, 0, "https://www.opengl.org/", 0, 0, SW_SHOW);
            }
            ImGui::SameLine();
            ImGui::Text("Version: %s", glGetString(GL_VERSION));

            if (ImGui::Button("Glew"))
            {
                ShellExecute(0, 0, "https://www.opengl.org/", 0, 0, SW_SHOW);
            }
            ImGui::SameLine();
            ImGui::Text("Version: %s", glewGetString(GLEW_VERSION));

            if (ImGui::Button("ImGui"))
            {
                ShellExecute(0, 0, "https://github.com/ocornut/imgui/", 0, 0, SW_SHOW);
            }
            ImGui::SameLine();
            ImGui::Text("Version: ");
            ImGui::SameLine();
            ImGui::Text(ImGui::GetVersion());

            if (ImGui::Button("MathGeoLib"))
            {
                ShellExecute(0, 0, "https://github.com/juj/MathGeoLib", 0, 0, SW_SHOW);
            }
            ImGui::SameLine();
            ImGui::Text("Version 1.5");

            if (ImGui::Button("DevIL"))
            {
                ShellExecute(0, 0, "http://openil.sourceforge.net/", 0, 0, SW_SHOW);
            }
            ImGui::SameLine();
            ImGui::SameLine();
            ImGui::Text("Version v1.8.0");

            ImGui::SeparatorText("LICENSE");
            ImGui::BulletText("MIT License");
            ImGui::NewLine();
            ImGui::Text("Copyright (c) Albert Martin & Adrian Ramirez");
            ImGui::NewLine();
            ImGui::TextWrapped("Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:");
            ImGui::NewLine();
            ImGui::TextWrapped("The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software");
            ImGui::NewLine();
            ImGui::TextWrapped("THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY. FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE");
            ImGui::NewLine();
            ImGui::TextWrapped("This is the MIT License, which is a very permissive license that allows users to do almost anything they want with the software, including using it for commercial purposes. The only requirement is that the copyright notice and permission notice must be included in all copies or substantial portions of the software.");

            if (ImGui::Button("Add 1 HP to the engine"))
            {
                cvCounter++;
            }
            ImGui::Text("Horse Power: %d", cvCounter);


            ImGui::EndMenu();
        }

        ImGui::SameLine(ImGui::GetWindowWidth() / 2 - 37);
        {
            //PLAY
            if (ImGui::Button(">", ImVec2(25, 25)))
            {
                //if (playPressed == true) {
                    if (App->IsRunning()) {
                        App->SetState(GameState::STOP);
                        ImGui::SetWindowFocus("Scene");
                        //pausePressed = false;
                        //playPressed = false;
                        App->LoadConfig();
                        App->scene->LoadSceneRequest();
                        App->physics->isWorldOn = false;
                        App->scene->DeleteGO();

                    }
                    else {
                        App->SetState(GameState::PLAY);
                        //playPressed = true;
                        ImGui::SetWindowFocus("Game");
                        App->SaveConfig();
                        App->scene->SaveSceneRequest();
                        App->physics->isWorldOn = true;

                    }
                //}
            }

            ImGui::SameLine();

            //PAUSE
            if (ImGui::Button("||", ImVec2(25, 25)))
            {
                //if (pausePressed == true) {

                    
                    if (App->IsRunning()) {
                        App->SetState(GameState::PAUSE);
                        App->physics->isWorldOn = true;
                       // pausePressed = false;
                    }
                    else
                    {
                       // pausePressed = true;
                        App->physics->isWorldOn = false;
                    }
                //}
            }

            ImGui::SameLine();

            //STOP
            if (ImGui::Button("[]", ImVec2(25, 25)))
            {
                //if (pausePressed == true) {

                    if (App->IsRunning()) {
                        App->SetState(GameState::STOP);
                        ImGui::SetWindowFocus("Scene");
                        App->LoadConfig();
                        App->scene->LoadSceneRequest();
                        //pausePressed = false;
                    }
                    else
                    {
                        //pausePressed = true;
                    }
                //}
            }
        }
        ImGui::EndMainMenuBar();
    }

    if (isActivatedHierarchy) {
        if (ImGui::Begin("Hierarchy"))
        {
            App->scene->PrintHierarchy(App->scene->rootGameObject, 0);
        }
        ImGui::End();
    }

    if (isActivatedInspector)
    {
        if (App->scene->selectedGameObj != nullptr)
        {
            if (ImGui::Begin("Inspector"))
            {
                App->scene->selectedGameObj->PrintOnInspector();
            }
            ImGui::End();
        }
    }
    
    if(isActivatedConsole)
    {
        if (ImGui::Begin("Console Log"))
        {
            ImGui::NewLine();

            int size = logVector->size();
            for (int i = 0; i < size; i++)
            {
                ImGui::Text((*logVector)[i].c_str());
            }

            
        }
        ImGui::End();
    }

    if (isActivatedGameWindow)
    {
        GameWindow();
    }
    if (isActivatedSceneWindow)
    {
        SceneWindow();
    }

    if (isActivatedDemo) 
    {
        ImGui::ShowDemoWindow();
    }
    
    RefreshRenderSettings();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_S) == KEY_UP)
    {
        App->scene->SaveSceneRequest();
    }

    return ret;
}

void ModuleEditor::RefreshRenderSettings()
{
    if (lightning)
    {
        glEnable(GL_LIGHTING);
    }
    else
    {
        glDisable(GL_LIGHTING);
    }

    if (isCullFace)
    {
        glEnable(GL_CULL_FACE);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
    if (depthTest)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
    if (colorMat)
    {
        glEnable(GL_COLOR_MATERIAL);
    }
    else
    {
        glDisable(GL_COLOR_MATERIAL);
    }
    if (isWireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    if (texture2D)
    {
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
    }
}

bool ModuleEditor::CleanUp()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    RELEASE(logVector);

    return true;
}

void ModuleEditor::GameWindow()
{
    ImGui::Begin("Game", 0, ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavFocus);
    sceneWindowSize = ImGui::GetContentRegionAvail();
    ImVec2 newWinSize = sceneWindowSize;
    newWinSize.x = (newWinSize.y / 9.0f) * 16.0f;

    float uvOffset = (sceneWindowSize.x - newWinSize.x) / 2.0f;
    uvOffset /= newWinSize.x;

    if (App->renderer3D->GetMainCamera() != nullptr)
        ImGui::Image((ImTextureID)App->renderer3D->GetMainCamera()->cameraBuffer, sceneWindowSize, ImVec2(-uvOffset, 1), ImVec2(1 + uvOffset, 0));

    ImGui::End();

}

void ModuleEditor::SceneWindow()
{
    ImGui::Begin("Scene");

    App->editor->guizmoWindowPos = ImGui::GetWindowPos();
    App->editor->guizmoOffset = ImGui::GetFrameHeight() / 2;
    App->editor->guizmoSize = ImGui::GetContentRegionAvail();

    sceneWindowSize = ImGui::GetContentRegionAvail();

    ImVec2 newWinSize = sceneWindowSize;
    newWinSize.x = (newWinSize.y / 9.0f) * 16.0f;

    float uvOffset = (sceneWindowSize.x - newWinSize.x) / 2.0f;
    uvOffset /= newWinSize.x;

    ImGui::Image((ImTextureID)App->camera->camera->cameraBuffer, sceneWindowSize, ImVec2(-uvOffset, 1), ImVec2(1 + uvOffset, 0));

    MousePicking();

    DrawGuizmos();

    ImGui::End();
}

void ModuleEditor::MousePicking()
{
    if (ImGui::IsMouseClicked(0) && App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT && ImGui::IsWindowHovered() && !ImGuizmo::IsOver())
    {
        std::vector<GameObject*> pickedGameObject;

        ImVec2 mousePos = ImGui::GetMousePos();
        /*LOG("Mouse Pos: %.2f, %.2f", mousePos.x, mousePos.y);*/

        ImVec2 normalPosition = NormMousePos(ImGui::GetWindowPos().x,
            ImGui::GetWindowPos().y + ImGui::GetFrameHeight(),
            ImGui::GetWindowSize().x,
            ImGui::GetWindowSize().y - ImGui::GetFrameHeight(), mousePos);

        // Offset to fix error
        normalPosition.x -= .5f;
        normalPosition.y -= .5f;

        /*LOG("Normalized Pos: %.2f, %.2f", norm.x, norm.y);*/

        LineSegment picking = App->camera->camera->frustum.UnProjectLineSegment(normalPosition.x, normalPosition.y);
        App->renderer3D->ls = picking;

        for (size_t i = 0; i < App->geoLoader->meshes.size(); i++)
        {
            if (picking.Intersects(App->geoLoader->meshes[i]->OBB_box))
            {
                //LOG("%d", App->geoLoader->meshes[i]->num_vertex);

                if (App->geoLoader->meshes[i]->owner != nullptr)
                    pickedGameObject.push_back(App->geoLoader->meshes[i]->owner);
            }
        }

        float currentDist;
        float minDist = 0;

        for (size_t j = 0; j < pickedGameObject.size(); j++) {

            C_Mesh* gObjMesh = pickedGameObject[j]->GetMeshComponent();
            if (gObjMesh != nullptr) {

                for (size_t i = 0; i < gObjMesh->meshes.size(); i++) {

                    Mesh* mesh = gObjMesh->meshes[i];
                    float4x4 matTrans = pickedGameObject[j]->mTransform->getGlobalMatrix().Transposed();

                    if (mesh->num_index > 9) {
                        for (size_t b = 0; b < mesh->num_index; b += 3) {

                            float* t1 = &mesh->vertex[mesh->index[b] * VERTEX_ARGUMENTS];
                            float* t2 = &mesh->vertex[mesh->index[b + 1] * VERTEX_ARGUMENTS];
                            float* t3 = &mesh->vertex[mesh->index[b + 2] * VERTEX_ARGUMENTS];

                            float4 tr1 = matTrans * float4(*t1, *(t1 + 1), *(t1 + 2), 1);
                            float4 tr2 = matTrans * float4(*t2, *(t2 + 1), *(t2 + 2), 1);
                            float4 tr3 = matTrans * float4(*t3, *(t3 + 1), *(t3 + 2), 1);

                            float3 tri1 = float3(tr1.x, tr1.y, tr1.z);
                            float3 tri2 = float3(tr2.x, tr2.y, tr2.z);
                            float3 tri3 = float3(tr3.x, tr3.y, tr3.z);

                            Triangle triangle(tri1, tri2, tri3);

                            if (picking.Intersects(triangle, &currentDist, nullptr))
                            {
                                if (minDist == 0) {
                                    minDist = currentDist;
                                    App->scene->SetGameObjectSelected(pickedGameObject[j]);
                                    continue;
                                }
                                if (currentDist < minDist) {
                                    minDist = currentDist;
                                    App->scene->SetGameObjectSelected(pickedGameObject[j]);
                                }

                            }
                        }
                    }
                }
            }

        }
        if (pickedGameObject.size() == 0) App->scene->SetGameObjectSelected(nullptr);
        pickedGameObject.clear();
    }
}

void ModuleEditor::DrawGuizmos()
{
    if (App->scene->selectedGameObj == nullptr) return;
    C_Transform* transform = App->scene->selectedGameObj->mTransform;
    if (transform == nullptr) return;

    //ImGuizmo::Enable(true);


    ImGuizmo::SetDrawlist();

    float x = ImGui::GetWindowPos().x;
    float y = ImGui::GetWindowPos().y;
    float w = sceneWindowSize.x;
    float h = sceneWindowSize.y;
    //Guizmo

    float4x4 aux = transform->getGlobalMatrix();

    ImGuizmo::MODE finalMode = (App->camera->operation == ImGuizmo::OPERATION::SCALE ? ImGuizmo::MODE::LOCAL : App->camera->mode);

    ImGuizmo::SetRect(x, y, w, h);
    if (ImGuizmo::Manipulate(App->camera->camera->GetViewMatrix(), App->camera->camera->GetProjetionMatrix(), App->camera->operation, finalMode, &aux.v[0][0]))
    {
        aux.Transpose();
        transform->SetTransformMatrixW(aux);
    }
}

ImVec2 ModuleEditor::NormMousePos(float x, float y, float w, float h, ImVec2 p)
{
    ImVec2 normP;

    normP.x = (p.x - x) / w;
    normP.y = 1.0f - (p.y - y) / h;  // Invert the Y-axis
    return normP;
}

void ModuleEditor::AddHistogramData(const float aFPS, std::vector<float>& data_vector)
{
    if (data_vector.size() == data_vector.capacity())
    {

        for (unsigned int i = 0; i < data_vector.size(); i++)
        {
            if (i + 1 < data_vector.size())
            {
                float iCopy = data_vector[i + 1];
                data_vector[i] = iCopy;
            }
        }
        data_vector[data_vector.capacity() - 1] = aFPS;

    }
    else
    {
        data_vector.push_back(aFPS);
    }
}


void ModuleEditor::ConsoleLog(const char* tmp_string)
{
    if (logVector == nullptr) return;
    logVector->push_back(tmp_string);
}

void ModuleEditor::GetHardwareInfo()
{
    caps += (SDL_HasRDTSC()) ? "RDTSC," : "";
    caps += (SDL_HasMMX()) ? "MMX, " : "";
    caps += (SDL_HasSSE()) ? "SSE, " : "";
    caps += (SDL_HasSSE2()) ? "SSE2, " : "";
    caps += (SDL_HasSSE3()) ? "SSE3, " : "";
    caps += "\n";
    caps += (SDL_HasSSE41()) ? "SSE41, " : "";
    caps += (SDL_HasSSE42()) ? "SSE42, " : "";
    caps += (SDL_HasAVX()) ? "AVX, " : "";
    caps += (SDL_HasAltiVec()) ? "AltiVec, " : "";
    caps += (SDL_Has3DNow()) ? "3DNow, " : "";

    SDL_version version;
    SDL_GetVersion(&version);
    SDLVersion = std::to_string(version.major) + '.' + std::to_string(version.minor) + '.' + std::to_string(version.patch);
    CPUCount = SDL_GetCPUCount();
    CPUCache = SDL_GetCPUCacheLineSize();
    systemRAM = SDL_GetSystemRAM() / 1024.f;

    uint vendor, deviceId;
    std::wstring brand;
    unsigned __int64 videoMemBudget;
    unsigned __int64 videoMemUsage;
    unsigned __int64 videoMemAvailable;
    unsigned __int64 videoMemReserved;

    // GPU INFO TOO COMPLICATED
}

