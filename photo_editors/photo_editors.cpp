#include "photo_editors.h"

cv::Mat contr_func(cv::Mat brightness_image, float contr) {
    cv::Mat rgb_image;
    cv::cvtColor(brightness_image, rgb_image, cv::COLOR_BGR2RGB);

    cv::Mat contrast_image = cv::Mat::zeros(rgb_image.size(), rgb_image.type());

    rgb_image.convertTo(contrast_image, -1, contr, 0);

    return contrast_image;

}


cv::Mat brightness_func(cv::Mat image, float br) {
    cv::Mat brightness_image;
    cv::addWeighted(image, 0.5, image, br - 0.5, 0.0, brightness_image);
    return brightness_image;
}

cv::Mat colorful_temperature_func(cv::Mat rgb_image, float ct) {
    std::vector<cv::Mat> rgb_ct;
    cv::split(rgb_image, rgb_ct);
    if (ct >= 1.0f) {
        rgb_ct[0] *= ct;
    }
    else if (ct < 1.0f) {
        ct = 1.0f + (1.0f - ct);
        rgb_ct[2] *= ct;
        rgb_ct[1] *= ct;
    }
    cv::merge(rgb_ct, rgb_image);

    return rgb_image;
}

cv::Mat HSV_func(cv::Mat rgb_image, float hue, float sat, float exp) {
    cv::Mat hsv_image;
    cv::cvtColor(rgb_image, hsv_image, cv::COLOR_RGB2HSV);

    std::vector<cv::Mat> hsv_channels;
    cv::split(hsv_image, hsv_channels);
    hsv_channels[0] *= hue;
    hsv_channels[1] *= sat;
    hsv_channels[2] *= exp;

    cv::merge(hsv_channels, hsv_image);

    return hsv_image;
}

cv::Mat sharp_func(cv::Mat image_blur, float sharp) {
    cv::Mat image_sharp;
    float matr[9]{
                -0.0375 - 0.05 * sharp, -0.0375 - 0.05 * sharp, -0.0375 - 0.05 * sharp,
                -0.0375 - 0.05 * sharp, 1.3 + 0.4 * sharp, -0.0375 - 0.05 * sharp,
                -0.0375 - 0.05 * sharp, -0.0375 - 0.05 * sharp, -0.0375 - 0.05 * sharp
    };

    cv::Mat kernel_matrix = cv::Mat(3, 3, CV_32FC1, &matr);
    cv::filter2D(image_blur, image_sharp, 32, kernel_matrix);

    return image_sharp;
}


cv::Mat create_image(cv::Mat image, std::map<std::string, float>& par_map, int blur_par) {

    float sharp = par_map["sharpness"];
    sharp *= 7.0f;

    

    brightness_func(image, par_map["brightness"]);

    contr_func(brightness_func(image, par_map["brightness"]), par_map["contrast"]);

    colorful_temperature_func(contr_func(brightness_func(image, par_map["brightness"]), par_map["contrast"]), par_map["colorful temperature"]);
    
    HSV_func(colorful_temperature_func(contr_func(brightness_func(image, par_map["brightness"]), par_map["contrast"]), par_map["colorful temperature"]), par_map["hue"], par_map["saturation"], par_map["exposition"]);
    
    cv::Mat result_image_rgb;
    cv::cvtColor(HSV_func(colorful_temperature_func(contr_func(brightness_func(image, par_map["brightness"]), par_map["contrast"]), par_map["colorful temperature"]), par_map["hue"], par_map["saturation"], par_map["exposition"]), result_image_rgb, cv::COLOR_HSV2RGB);

    cv::Mat image_blur;
    
    cv::blur(result_image_rgb, image_blur, cv::Size(blur_par, blur_par));
    
    sharp_func(image_blur, sharp);

    cv::Mat result_image_rgba;
    cv::cvtColor(sharp_func(image_blur, sharp), result_image_rgba, cv::COLOR_RGB2RGBA);

    return result_image_rgba;
}

void create_texture(cv::Mat result_image_rgba) {

    GLuint texture_id;
    glGenTextures(1, &texture_id);

    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, result_image_rgba.cols, result_image_rgba.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, result_image_rgba.data);
    glBindTexture(GL_TEXTURE_2D, 0);

    ImTextureID texture = (void*)(intptr_t)texture_id;

    ImGui::Image(texture, ImVec2(result_image_rgba.cols, result_image_rgba.rows));

}


int main()
{

    /*std::ofstream file_save("save");*/

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "GLFW window", NULL, NULL);

    if (window == NULL)
        return 1;

    glfwMakeContextCurrent(window);
    gl3wInit();
    glfwSwapInterval(1);
    gladLoadGL();

    Editor();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGuiIO& io = ImGui::GetIO();
    (void)io;


    ImGuiWindowFlags windowflag = 0;
    windowflag |= ImGuiWindowFlags_NoBackground;
    windowflag |= ImGuiWindowFlags_NoTitleBar;
    windowflag |= ImGuiWindowFlags_MenuBar;


    io.Fonts->AddFontFromFileTTF("C:/photo_editors/IBMPlexSans-ExtraLight.ttf", 30.0f);

    const char* filters = "Image files (*.png){.png} (*.jpg){.jpg}";
    static bool sl_par = false;
    static bool t_par = false;
    static std::string filePathName = "";
    static float saturation = 1.0f;
    static float hue = 1.0f;
    static float exposition = 1.0f;
    static float shade = 1.0f;
    static float colorful_temperature = 1.0f;
    static float contrast = 1.0f;
    static float sharpness = 0.0f;
    static float vignette = 1.0f;
    static float brightness = 1.0f;
    static int blur = 1;



    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos({ 0, 0 });
        ImGui::SetNextWindowSize({ 1920, 1080 });
        ImGui::Begin("Photo editor (Main window)", NULL, windowflag);

        // menubar



        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open..", "Ctrl+O")) {

                    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose a File", filters, ".", 1, nullptr, ImGuiFileDialogFlags_Modal);
                    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".png", ImVec4(0.0f, 1.0f, 1.0f, 0.9f));
                    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".jpg", ImVec4(0.0f, 1.0f, 1.0f, 0.9f));

                    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
                    {
                        // action if OK
                        if (ImGuiFileDialog::Instance()->IsOk())
                        {
                            filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

                        }
                    }
                }

                if (ImGui::MenuItem("Save", "Ctrl+S")) { return 1; }

                if (ImGui::MenuItem("Close", "Ctrl+W")) { return 0; }

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

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }


        //Keys
        if ((ImGui::IsKeyDown(ImGuiKey_RightCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) && ImGui::IsKeyDown(ImGuiKey_O)) {

            /*ShellExecute(NULL, "open", "C:", NULL, NULL, 1);*/
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose a File", filters, ".", 1, nullptr, ImGuiFileDialogFlags_Modal);

            ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".png", ImVec4(0.0f, 1.0f, 1.0f, 0.9f));
            ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".jpg", ImVec4(0.0f, 1.0f, 1.0f, 0.9f));

            if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
            {
                // action if OK
                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                    std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

                }
            }
        }


        if ((ImGui::IsKeyDown(ImGuiKey_RightCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) && ImGui::IsKeyDown(ImGuiKey_W)) {
            return 0;
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


        if (ImGui::Button("Open File Dialog"))
            /*ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".", "Image Files (*.png)");*/

            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose a File", filters, ".", 1, nullptr, ImGuiFileDialogFlags_Modal);

        ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".png" ".jpg", ImVec4(0.0f, 1.0f, 1.0f, 0.9f));
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


        /*ImGui::SetCursorPos(ImVec2(10, ((ImGui::GetWindowHeight()) / 3) - 50));
        ImGui::Checkbox("Hide slider parametres", &(sl_par));
        ImGui::Checkbox("Hide input parametres", &(in_par));*/

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

            ImGui::SliderFloat("Vignette", &vignette, 0.0f, 2.0f);

            ImGui::SliderFloat("Brightness", &brightness, 0.0f, 2.0f);
            
            ImGui::SliderInt("Blurred", &blur, 1, 20);


            ImGui::EndGroup();
            ImGui::PopItemWidth();
        }

        else if (!(sl_par) && (t_par)) {
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
            
            ImGui::InputFloat("Vignette", &vignette);
            
            ImGui::InputFloat("Brightness", &brightness);
            
            ImGui::InputInt("Blurred", &blur);

            ImGui::EndGroup();
            ImGui::PopItemWidth();
        }

        ImGui::SetCursorPos(ImVec2(((ImGui::GetWindowWidth()) / 3), ((ImGui::GetWindowHeight()) / 3) - 50));
                                     
        std::map<std::string, float> par_map{ {"saturation", saturation}, {"hue", hue}, {"exposition", exposition}, {"shade", shade}, {"colorful temperature", colorful_temperature}, {"contrast", contrast}, {"sharpness", sharpness}, {"vignette", vignette}, {"brightness", brightness} };

        cv::Mat image = cv::imread(file_name.c_str());
        if (file_name != "") {
            if (blur <= 0) {
                blur = 1;
                ImGui::Begin("Image", NULL, ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

                create_texture(create_image(image, par_map, blur));

                ImGui::End();
            }
         
        }

        if ((ImGui::IsKeyDown(ImGuiKey_RightCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) && ImGui::IsKeyDown(ImGuiKey_S)) {
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Save Image", ".jpg", ".", 1, nullptr, ImGuiFileDialogFlags_Modal);

            // Check if dialog is open and user has selected a file
            if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", ImGuiFileDialogFlags_ConfirmOverwrite))
            {
                if (ImGuiFileDialog::Instance()->IsOk()) {
                    // Get selected file path
                    std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

                    // Get selected directory path
                    std::string directoryPath = ImGuiFileDialog::Instance()->GetCurrentPath();

                    // Get file name
                    std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();

                    // Combine directory path and file name to get full file path
                    std::string fullPath = directoryPath + "/" + fileName;

                    // Save image to selected file path
                    cv::imwrite(fullPath, create_image(image, par_map, blur));
                }
                
                
            }
        }

        ImGui::End();

        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.f, 0.f, 0.2, 1.0f);
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



// Create imgui file dialog


