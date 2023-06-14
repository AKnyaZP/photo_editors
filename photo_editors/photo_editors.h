#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "ImGuizmo.h"
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


class Editor {

public:
    Editor() = default;
    ~Editor() = default;



};

class Image {
public:
    //Image(float saturation_, float hue_, float exposition_, float shade_, float colorful_temperature_, float contrast_, float sharpness_, float brightness_, int blur_) {
    //    saturation = saturation_;
    //    hue = hue_;
    //    exposition = exposition_;
    //    shade = shade_;
    //    colorful_temperature = colorful_temperature_;
    //    contrast = contrast_;
    //    sharpness = sharpness_;
    //    brightness = brightness_;
    //    blur = blur_;
    //};
    Image() = default;
    Image(const Image&) = default;
    Image(Image&&) = default;
    ~Image() = default;
    /*Image(const Image& other);
    Image(Image&& other);
    Image& operator=(const Image& other);*/

    //std::string file_name = file_name_;
    //float saturation = saturation_;
    //float hue = hue_;
    //float exposition = exposition_;
    //float shade = shade_;
    //float colorful_temperature = colorful_temperature_;
    //float contrast = contrast_;
    //float sharpness = sharpness_;
    //float brightness = brightness_;
    //int blur = blur_;


    cv::Mat create_image(std::string file_name, float saturation, float hue, float exposition, float shade, float colorful_temperature, float contrast, float sharpness, float brightness, int blur) {
        cv::Mat image = cv::imread(file_name.c_str());

        float sharp = sharpness;
        sharp *= 7.0f;

        cv::Mat brightness_image;

        cv::addWeighted(image, 0.5, image, brightness - 0.5, 0.0, brightness_image);

        cv::Mat rgb_image;
        cv::cvtColor(brightness_image, rgb_image, cv::COLOR_BGR2RGB);

        cv::Mat contrast_image = cv::Mat::zeros(rgb_image.size(), rgb_image.type());

        rgb_image.convertTo(contrast_image, -1, contrast, 0);

        std::vector<cv::Mat> rgb_ct;
        cv::split(contrast_image, rgb_ct);
        if (colorful_temperature >= 1.0f) {
            rgb_ct[0] *= colorful_temperature;
        }
        else if (colorful_temperature < 1.0f) {
            //colorful_temperature = (1.0f - colorful_temperature);
            rgb_ct[2] *= colorful_temperature;
        }
        cv::merge(rgb_ct, rgb_image);

        cv::Mat hsv_image;
        cv::cvtColor(rgb_image, hsv_image, cv::COLOR_RGB2HSV);

        std::vector<cv::Mat> hsv_channels;
        cv::split(hsv_image, hsv_channels);
        hsv_channels[0] *= hue;
        hsv_channels[1] *= saturation;
        hsv_channels[2] *= exposition;

        cv::merge(hsv_channels, hsv_image);

        cv::Mat result_image_rgb;
        cv::cvtColor(hsv_image, result_image_rgb, cv::COLOR_HSV2RGB);

        cv::Mat image_blur;

        cv::blur(result_image_rgb, image_blur, cv::Size(blur, blur));

        cv::Mat image_sharp;
        float matr[9]{
                    -0.0375f - 0.05f * sharp, -0.0375f - 0.05f * sharp, -0.0375f - 0.05f * sharp,
                    -0.0375f - 0.05f * sharp, 1.3f + 0.4f * sharp, -0.0375f - 0.05f * sharp,
                    -0.0375f - 0.05f * sharp, -0.0375f - 0.05f * sharp, -0.0375f - 0.05f * sharp
        };

        cv::Mat kernel_matrix = cv::Mat(3, 3, CV_32FC1, &matr);
        cv::filter2D(image_blur, image_sharp, 32, kernel_matrix);

        cv::Mat result_image_rgba;
        cv::cvtColor(image_sharp, result_image_rgba, cv::COLOR_RGB2RGBA);

        return result_image_rgba;
    }

    ImTextureID create_texture(cv::Mat result_image_rgba) {

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

        return texture;
    }

    cv::Mat RGBAtoBGR(cv::Mat result_image_rgba) {
        cv::cvtColor(result_image_rgba, result_image_rgba, cv::COLOR_RGBA2BGR);
        return result_image_rgba;
    }

    void show_image_imgui(ImTextureID texture) {
        ImGui::Image(texture, ImGui::GetWindowSize());
    }
    void kill_texture(GLuint texture_id) {
        glDeleteTextures(1, &texture_id);
    }

};


//
//
//private:
//    std::string file_name_ = "";
//    float saturation_ = 1.0f;
//    float hue_ = 1.0f;
//    float exposition_ = 1.0f;
//    float shade_ = 1.0f;
//    float colorful_temperature_ = 1.0f;
//    float contrast_ = 1.0f;
//    float sharpness_ = 0.0f;
//    float brightness_ = 1.0f;
//    int blur_ = 1;




//cmake --install . --config release --prefix C:\Users\knyaz_ayotgwn\Desktop\install_for_pe
