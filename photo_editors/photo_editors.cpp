#include "photo_editors.h"

void contr_func(cv::Mat rgb_image, float contr) {
    std::vector<cv::Mat> rgb_contr;
    cv::split(rgb_image, rgb_contr);
    rgb_contr[0] *= contr;
    rgb_contr[1] *= contr;
    cv::merge(rgb_contr, rgb_image);
}

//void colorful_temperature_func(cv::Mat rgb_image, float ct) {
//    std::vector<cv::Mat> rgb;
//    cv::split(rgb_image, rgb);
//    rgb[0] *= ct;
//    rgb[2] *= ct;
//    cv::merge(rgb, rgb_image);
//}

void create_texture(std::string file_name, std::map<std::string, float>& par_map) {

    cv::Mat image = cv::imread(file_name.c_str());
    
    // Convert image to RGBA format
    cv::Mat rgb_image;
    cv::cvtColor(image, rgb_image, cv::COLOR_BGR2RGB);

    contr_func(rgb_image, par_map["contrast"]);

    std::vector<cv::Mat> rgb_ct;
    cv::split(rgb_image, rgb_ct);
    if (par_map["corful temperature"] <= 1.0f) {
        rgb_ct[0] *= par_map["colorful temperature"];
    }
    else if (par_map["colorful temperature"] >= 1.0f) {
        rgb_ct[2] *= par_map["colorful temperature"];
    }
    cv::merge(rgb_ct, rgb_image);

    /*cv::Mat sharp;
    addWeighted(rgb_image, 1.5, blurred, -0.5, par_map["sharpness"], sharp);*/
    //colorful_temperature_func(rgb_image, par_map["colorful_temperature"]);

    // Get image size
    cv::Mat hsv_image;
    cv::cvtColor(rgb_image, hsv_image, cv::COLOR_RGB2HSV);

    std::vector<cv::Mat> hsv_channels;
    cv::split(hsv_image, hsv_channels);
    hsv_channels[0] *= par_map["hue"];
    hsv_channels[1] *= par_map["saturation"];
    hsv_channels[2] *= par_map["exposition"];

    cv::merge(hsv_channels, hsv_image);


    cv::Mat result_image_rgb;
    cv::cvtColor(hsv_image, result_image_rgb, cv::COLOR_HSV2RGB);

   /* cv::Mat blurred;
    GaussianBlur(result_image_rgb, blurred, cv::Size(image.cols, image.rows), (double)par_map["blur"]);*/

    cv::Mat result_image_rgba;
    cv::cvtColor(result_image_rgb, result_image_rgba, cv::COLOR_RGB2RGBA);

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
 

    //io.Fonts->AddFontFromFileTTF("C://photo_editors/IBMPlexSans-ExtraLight.ttf", 30.0f);

    const char* filters = "Image files (*.png){.png} (*.jpg){.jpg}";
    static bool par = false;
    static std::string filePathName = "";
    static float saturation = 1.0f;
    static float hue = 1.0f;
    static float exposition = 1.0f;
    static float shade = 1.0f;
    static float colorful_temperature = 1.0f;
    static float contrast = 1.0f;
    static float sharpness = 1.0f;
    static float vignette = 1.0f;
    static float blur = 0.0f;
    
    

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

     /*   cv::Mat image = cv::imread(file_name);
        cv::Mat hsv_image;
        cv::cvtColor(image, hsv_image, cv::COLOR_BGR2HSV);*/

        
        


        ImGui::SetCursorPos(ImVec2( 10, ((ImGui::GetWindowHeight()) / 3) - 50));
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
           
            ImGui::SliderFloat("Sharpness", &sharpness, 0.0f, 2.0f);

            ImGui::SliderFloat("Vignette", &vignette, 0.0f, 2.0f);

            ImGui::SliderFloat("Blurred", &blur, 0.0f, 2.0f);

            ImGui::EndGroup();
            ImGui::PopItemWidth();
        }

        ImGui::SetCursorPos(ImVec2(((ImGui::GetWindowWidth()) / 3), ((ImGui::GetWindowHeight()) / 3) - 50));
        
        std::map<std::string, float> par_map{ {"saturation", saturation}, {"hue", hue}, {"exposition", exposition}, {"shade", shade}, {"colorful temperature", colorful_temperature}, {"contrast", contrast}, {"sharpness", sharpness}, {"vignette", vignette}, {"blurred", blur}};
      
        //float zoom_level = 1.0f; // Значение масштаба
        //ImGui::SetNextWindowContentSize(ImVec2(width * zoom_level, height * zoom_level));
        if (file_name != "") {
            
            

           /* cv::imshow("im", result_image);
            cv::waitKey(0);*/
            
            ImGui::Begin("Image", NULL, ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
            create_texture(file_name, par_map);
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




