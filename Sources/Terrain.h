#pragma once

//GL
#include <GL/glew.h>

//GLM
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

//Mes classes
#include "Shader.hpp"
#include "helpOpenGL.h"
#include "HeightGenerator.h"

#include <vector>
#include <unordered_map>

#define TARRAIN_WIDTH = 800;
#define VERTEX_COUNT = 128;

class Terrain{

public:
	Terrain(float x, float z, float size, size_t len, size_t col, Shader* shader, Shader* waterShader, HeightGenerator* heightGenerator, bool mainThread = true);

	void generateTerrain(bool mainThread);
	void generateWater();
	void sendData();
	void calculateNormals();
	glm::vec3 calculateNormal(int x1, int z1, int x2, int z2, int x3, int z3);
	glm::vec3 calculateNormal(int face);

	float getY(float x, float z);

	void addNormal(int x, int z);
	glm::vec3 getNormal(int x, int z);

	void draw();
	void drawWater();

private:
	float x_;
	float z_;
	float size_;

	size_t len_;
	size_t col_;

	std::vector<float> data_;
	
	std::vector<float> position_;
	std::vector<float> normal_;
	std::vector<float> texPos_;
	std::vector<int> indices_;
	size_t nbVertices_;
	//On stoque les y
	std::vector<float> heightMap_;
	
	//Les id du terrain
	unsigned int VAO_;
	unsigned int VBO_;
	unsigned int VBO2_;
	unsigned int VBO3_;
	unsigned int EBO_;


	//Si le chunk est assez bas pour avoir de l'eau
	bool isWater;
	std::vector<float> waterPosition_;
	std::vector<float> waterNormal_;
	std::vector<float> waterTexPos_;
	std::vector<int> waterIndices_;
	size_t waterNbVertices_;

	//Les id de l'eau
	unsigned int waterVAO_;
	unsigned int waterVBO_;
	unsigned int waterVBO2_;
	unsigned int waterVBO3_;
	unsigned int waterEBO_;


	Shader* shader_;
	Shader* waterShader_;


	HeightGenerator* heightGenerator_;
};

