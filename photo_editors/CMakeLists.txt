# TODO: Add tests and install targets if needed.
include_directories(.)
add_executable (photo_editors "photo_editors.cpp"  "dialog/ImGuiFileDialog/ImGuiFileDialog.cpp" "photo_editors.h")
target_link_libraries(photo_editors imgui::imgui glfw glad::glad unofficial::gl3w::gl3w)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
#target_link_libraries(photo_editors PRIVATE opencv_ml opencv_dnn opencv_core opencv_flann)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET photo_editors PROPERTY CXX_STANDARD 20)
endif()
