#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <iostream>



#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include "text_renderer.hpp"

int draw();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);