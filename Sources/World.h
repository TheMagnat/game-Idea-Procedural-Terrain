#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <thread>
#include <mutex>

//GLM
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Camera.hpp"
#include "Terrain.h"
#include "HeightGenerator.h"
#include "helpOpenGL.h"

class World{

public:
	World(float size, std::string const& vertex, std::string const& fragment, std::string const& geometry, Camera* camPtr, unsigned int texGrass_);
	~World();

	void addTerrain(int x, int z, size_t len, size_t col);
	void addTerrainThread(int x, int z, size_t len, size_t col);

	float getY(float x, float z);

	void terrainCheckThread(size_t nbChunk);
	void update(size_t nbChunk);

	void draw();

private:
	Shader shader_;
	Shader waterShader_;
	Camera* camera_;

	std::unordered_map<int, std::unordered_map<int, Terrain>> allTerrain_;
	std::unordered_map<int, std::unordered_set<int>> reservedTerrain_;

	std::vector<std::pair<int, int>> toActivate;

	//Ecran
	size_t width_;
	size_t height_;

	float size_;

	//Texture
	unsigned int texGrass_;
	unsigned int texRock_;
	unsigned int texSnow_;

	HeightGenerator heightGenerator_;


	//Mutex
	std::mutex checkTerrainMutex;
	std::mutex editDataMutex;

};
