#include "photo_editors.h"

int main()
{

    /*std::ofstream file_save("save");*/

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Photo Editor", NULL, NULL);

    if (window == NULL)
        return 1;

    glfwMakeContextCurrent(window);
    gl3wInit();
    glfwSwapInterval(1);
    gladLoadGL();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGuiIO& io = ImGui::GetIO();
    (void)io;


    ImGuiWindowFlags windowflag = 0;
    windowflag |= ImGuiWindowFlags_NoBackground;
    windowflag |= ImGuiWindowFlags_NoTitleBar;
    windowflag |= ImGuiWindowFlags_MenuBar;
    windowflag |= ImGuiWindowFlags_NoBringToFrontOnFocus;

    io.FontGlobalScale = 2.0f;
    //io.Fonts->AddFontFromFileTTF(default, 30.0f);

    //Image imageClass(static float saturation = 1.0f, static float hue = 1.0f, static float exposition = 1.0f, float shade = 1.0f, float colorful_temperature = 1.0f, float contrast = 1.0f, float sharpness = 0.0f, float brightness = 1.0f, float blur = 1);
    Image image;
    const char* filters = "Image files (*.jpg){.jpg} (*.png){.png}";
    bool sl_par = false;
    bool t_par = false;
    bool dialog = false;
    std::string filePathName = "";
    float saturation = 1.0f;
    float hue = 1.0f;
    float exposition = 1.0f;
    float shade = 1.0f;
    float colorful_temperature = 1.0f;
    float contrast = 1.0f;
    float sharpness = 0.0f;
    float vignette = 1.0f;
    float brightness = 1.0f;
    int blur = 1;
    bool flag_for_image = true;
    bool flag_for_safe_dialog = false;
    bool flag_for_tool = false;
    int int_random_num_for_name = std::rand() % 100 + 1;
    std::string string_random_num_for_name = std::to_string(int_random_num_for_name);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos({ 0, 0 });
        ImGui::SetNextWindowSize({ 1920, 1080 });

       
        /*ImGui::Begin("Begin with PhotoEditor", NULL, NULL);
        create_video("C:/Users/knyaz_ayotgwn/Downloads/sample - 10s.mp4");
        ImGui::End();*/
         

        ImGui::Begin("PhotoEditor (Main window)", NULL, windowflag);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("file")) {
                if (ImGui::MenuItem("Open...", "Ctrl+O")) {

                    dialog = true;
                }

                if (ImGui::MenuItem("Save", "Ctrl+S")) { flag_for_safe_dialog = true; }

                if (ImGui::MenuItem("Close", "Ctrl+W")) { flag_for_tool = true; }

               

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("settings")) {
                if (ImGui::MenuItem("Sliders parametres", "Ctrl+P")) {
                    if (!sl_par && !t_par)
                        sl_par = true;
                    else
                        sl_par = false;
                }

                if (ImGui::MenuItem("Text parametres", "Ctrl + T")) {
                    if (sl_par == false && t_par == false) {
                        t_par = true;
                    }
                    else {
                        t_par = false;
                    }
                }
                if (ImGui::MenuItem("Default value for parametres", "Ctrl + L")) {
                    saturation = 1.0f;
                    hue = 1.0f;
                    exposition = 1.0f;
                    shade = 1.0f;
                    colorful_temperature = 1.0f;
                    contrast = 1.0f;
                    sharpness = 0.0f;
                    vignette = 1.0f;
                    brightness = 1.0f;
                    blur = 1;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }


        //Keys
        if ((ImGui::IsKeyDown(ImGuiKey_RightCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) && ImGui::IsKeyDown(ImGuiKey_O)) {
            dialog = true;
        }

        if ((ImGui::IsKeyDown(ImGuiKey_RightCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) && ImGui::IsKeyDown(ImGuiKey_S))
        {
            flag_for_safe_dialog = true;
        }

        if ((ImGui::IsKeyDown(ImGuiKey_RightCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) && ImGui::IsKeyDown(ImGuiKey_W)) {
            flag_for_tool = true;
        }

        if ((ImGui::IsKeyDown(ImGuiKey_RightCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) && ImGui::IsKeyDown(ImGuiKey_P)) {
            if (!sl_par && !t_par)
                sl_par = true;
            else
                sl_par = false;
        }

        if ((ImGui::IsKeyDown(ImGuiKey_RightCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) && ImGui::IsKeyDown(ImGuiKey_T)) {
            if (sl_par == false && t_par == false) {
                t_par = true;
            }
            else {
                t_par = false;
            }
        }

        if ((ImGui::IsKeyDown(ImGuiKey_RightCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) && ImGui::IsKeyDown(ImGuiKey_L)) {
            saturation = 1.0f;
            hue = 1.0f;
            exposition = 1.0f;
            shade = 1.0f;
            colorful_temperature = 1.0f;
            contrast = 1.0f;
            sharpness = 0.0f;
            vignette = 1.0f;
            brightness = 1.0f;
            blur = 1;
        }




        if (dialog) {
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose a File", filters, ".", 1, nullptr, ImGuiFileDialogFlags_Modal);

            dialog = false;
        }

        ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".jpg" ".png", ImVec4(0.0f, 1.0f, 1.0f, 0.9f));
        // display
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", ImGuiFileDialogFlags_ConfirmOverwrite))
        {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

                // action
            }

            // close
            ImGuiFileDialog::Instance()->Close();
        }

        std::string file_name = filePathName;

        if ((sl_par)) {

            ImGui::SetCursorPos(ImVec2(10, (ImGui::GetWindowHeight()) / 3));

            ImGui::PushItemWidth(500);
            ImGui::BeginGroup();

            ImGui::Text("Sliders parametres");

            ImGui::SliderFloat("Saturation", &saturation, 0.0f, 2.0f);

            ImGui::SliderFloat("Hue", &hue, 0.0f, 2.0f);

            ImGui::SliderFloat("Exposition", &exposition, 0.0f, 2.0f);

            ImGui::SliderFloat("Shade", &shade, 0.0f, 2.0f);

            ImGui::SliderFloat("Colorful temperature", &colorful_temperature, 0.0f, 2.0f);

            ImGui::SliderFloat("Contrast", &contrast, 0.0f, 2.0f);

            ImGui::SliderFloat("Sharpness", &sharpness, -2.0f, 2.0f);

            ImGui::SliderFloat("Brightness", &brightness, 0.0f, 2.0f);

            ImGui::SliderInt("Blurred", &blur, 1, 20);


            ImGui::EndGroup();
            ImGui::PopItemWidth();
        }

        if (!(sl_par) && (t_par)) {
            ImGui::SetCursorPos(ImVec2(10, (ImGui::GetWindowHeight()) / 3));

            ImGui::PushItemWidth(500);
            ImGui::BeginGroup();

            ImGui::Text("Text parametres");

            ImGui::InputFloat("Saturation", &saturation);

            ImGui::InputFloat("Hue", &hue);

            ImGui::InputFloat("Exposition", &exposition);

            ImGui::InputFloat("Shade", &shade);

            ImGui::InputFloat("Colorful temperature", &colorful_temperature);

            ImGui::InputFloat("Contrast", &contrast);

            ImGui::InputFloat("Sharpness", &sharpness);

            ImGui::InputFloat("Brightness", &brightness);

            ImGui::InputInt("Blurred", &blur);

            ImGui::EndGroup();
            ImGui::PopItemWidth();
        }

        ImGui::SetCursorPos(ImVec2(((ImGui::GetWindowWidth()) / 3), ((ImGui::GetWindowHeight()) / 3) - 50));

        if (file_name != "") {
            if (blur <= 0) {
                blur = 1;
            }

            ImGui::Begin("Image", NULL, ImGuiWindowFlags_NoCollapse);

           /* cv::imshow("yo", image.RGBAtoBGR(image.create_image(file_name, saturation, hue, exposition, shade, colorful_temperature, contrast, sharpness, brightness, blur)));
            cv::waitKey(0);*/
            image.show_image_imgui(image.create_texture(image.create_image(file_name, saturation, hue, exposition, shade, colorful_temperature, contrast, sharpness, brightness, blur)));

            ImGui::End();

        }
        //cv::Mat image_for_editor = image.create_image(file_name, saturation, hue, exposition, shade, colorful_temperature, contrast, sharpness, brightness, blur);
        
        if (flag_for_tool) {
            ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x / 2.0f, ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMax().y / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

            ImGui::OpenPopup("Notification");
        }

        if (ImGui::BeginPopupModal("Notification", &flag_for_tool))
        {
            ImGui::Text("Are you sure you want to leave");
            ImGui::Separator();

            if (ImGui::Button("Exit")) {
                return 0;
            }

            if (ImGui::Button("Save")) {
                flag_for_safe_dialog = true;
                flag_for_tool = false;
            }

            ImGui::EndPopup();
        }
            

        //cv::cvtColor(image.create_image(file_name, saturation, hue, exposition, shade, colorful_temperature, contrast, sharpness, brightness, blur), image.create_image(file_name, saturation, hue, exposition, shade, colorful_temperature, contrast, sharpness, brightness, blur), cv::COLOR_RGBA2BGR);

        if (flag_for_safe_dialog) {
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFolderDlg", "Choose Folder", nullptr, ".");
            // Отобразить диалоговое окно сохранения файла  
        }

        if (ImGuiFileDialog::Instance()->Display("ChooseFolderDlg")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                std::string FPName = ImGuiFileDialog::Instance()->GetFilePathName();
                /*std::string FName = ImGuiFileDialog::Instance()->GetCurrentFileName();

                ImGui::InputText("FileName", FName);*/
                
                cv::imwrite(FPName + "/" + string_random_num_for_name + "img.jpg", image.RGBAtoBGR(image.create_image(file_name, saturation, hue, exposition, shade, colorful_temperature, contrast, sharpness, brightness, blur)));
            }
            ImGuiFileDialog::Instance()->Close();
            flag_for_safe_dialog = false;
        }



        ImGui::End();

        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.f, 0.f, 0.3, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}




