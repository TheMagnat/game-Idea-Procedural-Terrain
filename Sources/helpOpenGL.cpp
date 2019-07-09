#include "pch.h"

#include "helpOpenGL.h"
#include <GL/glew.h>

#include <iostream>


void createVAO(unsigned int& VAO, unsigned int& VBO, std::vector<float> const& data, std::vector<size_t> const& para) {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);


	size_t somme(0);
	for (size_t i(0), taille(para.size()); i < taille; ++i) {
		somme += para[i];
	}

	for (size_t i(0), taille(para.size()); i < taille; ++i) {

		glVertexAttribPointer(i, para[i], GL_FLOAT, GL_FALSE, somme * sizeof(float), (void*)0);
		glEnableVertexAttribArray(i);

	}


}




void bindIndicesBuffer(unsigned int& EBO, std::vector<int> indices) {
	
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

}

void storeDataInAttributeList(unsigned int VBO, int attributeNumber, int coordinateSize, std::vector<float> const& data) {
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(attributeNumber, coordinateSize, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(attributeNumber);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void createVAO(unsigned int& VAO, unsigned int& VBO1, unsigned int VBO2, unsigned int VBO3, unsigned int& EBO, std::vector<float> const& pos, std::vector<float> const& norm, std::vector<float> const& texCoord, std::vector<int> const& indices) {

	//VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	bindIndicesBuffer(EBO, indices);

	storeDataInAttributeList(VBO1, 0, 3, pos);
	storeDataInAttributeList(VBO2, 1, 3, norm);
	storeDataInAttributeList(VBO3, 2, 2, texCoord);

	glBindVertexArray(0);
}

void genMesh(size_t len, size_t col, std::vector<float>& dataToFill) {

	//Nombre de carré (len * col), 2 triangles par carré, 3 points par triangle.
	dataToFill.reserve(len*col * 2 * 3);

	//On pourrait utiliser widthTexStep pour calculer widthStep et pareille pour height mais pour etre le plus précis possible sur les float je prefere le recalculer
	float widthStep(2.0f / len), heightStep(2.0f / col);
	float widthTexStep(1.0f / len), heightTexStep(1.0f / col);

	for (size_t i(0); i < col; ++i) {
		for (size_t j(0); j < len; ++j) {

			float actualX((i / (float)col)*2.0f - 1.0f), actualZ((j / (float)len)*2.0f - 1.0f);
			float actualXTex(i / (float)col), actualYTex(j / (float)len);

			//Premier Triangle
			//Premier Point
			dataToFill.push_back(actualX); //X
			dataToFill.push_back(0); //Y
			dataToFill.push_back(actualZ); //Z

			dataToFill.push_back(actualXTex); //TexX
			dataToFill.push_back(actualYTex); //TexY


			//Second Point
			dataToFill.push_back(actualX + widthStep); //X
			dataToFill.push_back(0); //Y
			dataToFill.push_back(actualZ); //Z

			dataToFill.push_back(actualXTex + widthTexStep); //TexX
			dataToFill.push_back(actualYTex); //TexY


			//Troisieme point
			dataToFill.push_back(actualX); //X
			dataToFill.push_back(0); //Y
			dataToFill.push_back(actualZ + heightStep); //Z

			dataToFill.push_back(actualXTex); //TexX
			dataToFill.push_back(actualYTex + heightTexStep); //TexY


			//Second Triangle
			//Premier Point
			dataToFill.push_back(actualX); //X
			dataToFill.push_back(0); //Y
			dataToFill.push_back(actualZ + heightStep); //Z

			dataToFill.push_back(actualXTex); //TexX
			dataToFill.push_back(actualYTex + heightTexStep); //TexY


			//Second Point
			dataToFill.push_back(actualX + widthStep); //X
			dataToFill.push_back(0); //Y
			dataToFill.push_back(actualZ + heightStep); //Z

			dataToFill.push_back(actualXTex + widthTexStep); //TexX
			dataToFill.push_back(actualYTex + heightTexStep); //TexY


			//Troisieme point
			dataToFill.push_back(actualX + widthStep); //X
			dataToFill.push_back(0); //Y
			dataToFill.push_back(actualZ); //Z

			dataToFill.push_back(actualXTex + widthTexStep); //TexX
			dataToFill.push_back(actualYTex); //TexY


		}
	}

}

/**
*   Fonction pour charger une texture 2D simple.
*   Retourne l'identifiant openGL de celle-ci.
*   Note : version sans canal alpha.
**/
unsigned int loadTexture(std::string const& file) {
	unsigned int textureID;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load(file.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture " << file << std::endl;
	}
	stbi_image_free(data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.3);

	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}