#pragma once

#include <iostream>
#include <map>
#include <string>

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "shader.hpp"


void RenderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color);

/// Holds all state information relevant to a character as loaded using FreeType
