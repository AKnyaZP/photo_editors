#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include <Windows.h>
#include <iosfwd>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#define GL_SILENCE_DEPRECATION
#define ICON_IGFD_FILE_PIC
#define USE_IMGUI_TABLES  
#include<glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/gl3w.h>
#define STB_IMAGE_IMPLEMENTATION
#include <dialog/ImGuiFileDialog/stb/stb_image.h>
#include <dialog/ImGuiFileDialog/ImGuiFileDialog.h>
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <string>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <map>



class Editor {

public:
	Editor() {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplOpenGL3_Init("#version 330");
	};
	~Editor() = default;



};
//
//class Sliders : private Editor {
//	void sliders_parametres();  
//};
//
//	

//void save_function() {
//    if (file_save.is_open()) {
//
//    }
//}

