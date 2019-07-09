#pragma once

#include <vector>
#include <string>

#include "stb_image.h"


void genMesh(size_t len, size_t col, std::vector<float>& dataToFill);
void createVAO(unsigned int& VAO, unsigned int& VBO, std::vector<float> const& data, std::vector<size_t> const& para);
void createVAO(unsigned int& VAO, unsigned int& VBO1, unsigned int VBO2, unsigned int VBO3, unsigned int& EBO, std::vector<float> const& pos, std::vector<float> const& norm, std::vector<float> const& texCoord, std::vector<int> const& indices);
unsigned int loadTexture(std::string const& file);
