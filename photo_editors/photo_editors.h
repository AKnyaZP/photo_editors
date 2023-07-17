#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include <iosfwd>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#define GL_SILENCE_DEPRECATION
#define USE_IMGUI_TABLES  
#include<glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/gl3w.h>
#include <dialog/ImGuiFileDialog/ImGuiFileDialog.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <random>
#include <memory>
#include <ctime>
#include <algorithm>

class Editor {

public:
    Editor() = default;
    Editor(const Editor&) {}
    ~Editor() = default;


};





//cmake --install . --config release --prefix C:\Users\knyaz_ayotgwn\Desktop\install_for_pe