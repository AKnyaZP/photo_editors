#include "photo_editors.h"

cv::Mat contr_func(cv::Mat brightness_image, float contr) {
    cv::Mat rgb_image;
    cv::cvtColor(brightness_image, rgb_image, cv::COLOR_BGR2RGB);
    std::vector<cv::Mat> rgb_contr;
    cv::split(rgb_image, rgb_contr);
    rgb_contr[0] *= contr;
    rgb_contr[1] *= contr;
    cv::merge(rgb_contr, rgb_image);
    return rgb_image;
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
    else if (ct <= 1.0f) {
        rgb_ct[2] *= (1.0f - ct) * 10.0f;
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

cv::Mat sharp_func(cv::Mat image_blur, double sharp) {
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
void create_texture(std::string file_name, std::map<std::string, float>& par_map, int blur_par) {

    double sharp = (double)par_map["sharpness"];
    sharp *= 7.0;

    cv::Mat image = cv::imread(file_name.c_str());

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
    ImGuiIO& io = ImGui::GetIO(); (void)io;


    ImGuiWindowFlags windowflag = 0;
    windowflag |= ImGuiWindowFlags_NoBackground;
    windowflag |= ImGuiWindowFlags_NoTitleBar;
    windowflag |= ImGuiWindowFlags_MenuBar;


    io.Fonts->AddFontFromFileTTF("C://photo_editors/IBMPlexSans-ExtraLight.ttf", 30.0f);

    const char* filters = "Image files (*.png){.png} (*.jpg){.jpg}";
    static bool par = false;
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

        if ((ImGui::IsKeyDown(ImGuiKey_RightCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) && ImGui::IsKeyDown(ImGuiKey_S)) {
            return 1;
        }

        if ((ImGui::IsKeyDown(ImGuiKey_RightCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) && ImGui::IsKeyDown(ImGuiKey_W)) {
            return 0;
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


        ImGui::SetCursorPos(ImVec2(10, ((ImGui::GetWindowHeight()) / 3) - 50));
        ImGui::Checkbox("Hide parametres", &(par));

        if (!(par)) {

            ImGui::SetCursorPos(ImVec2(10, (ImGui::GetWindowHeight()) / 3));

            ImGui::PushItemWidth(500);
            ImGui::BeginGroup();

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

        ImGui::SetCursorPos(ImVec2(((ImGui::GetWindowWidth()) / 3), ((ImGui::GetWindowHeight()) / 3) - 50));

        std::map<std::string, float> par_map{ {"saturation", saturation}, {"hue", hue}, {"exposition", exposition}, {"shade", shade}, {"colorful temperature", colorful_temperature}, {"contrast", contrast}, {"sharpness", sharpness}, {"vignette", vignette}, {"brightness", brightness} };

        //float zoom_level = 1.0f; // Значение масштаба
        //ImGui::SetNextWindowContentSize(ImVec2(width * zoom_level, height * zoom_level));
        if (file_name != "") {

            ImGui::Begin("Image", NULL, ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
            create_texture(file_name, par_map, blur);
            ImGui::End();

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



