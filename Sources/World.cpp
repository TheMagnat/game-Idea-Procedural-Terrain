#include "pch.h"

#include "World.h"



World::World(float size, std::string const& vertex, std::string const& fragment, std::string const& geometry, Camera* camPtr, unsigned int texGrass) : size_(size), camera_(camPtr), texGrass_(texGrass), texRock_(loadTexture("Textures/rock.jpg")), texSnow_(loadTexture("Textures/snow.jpg")) {
	
	if (geometry.empty()) {
		shader_.load(vertex.data(), fragment.data());
	}
	else {
		shader_.load(vertex.data(), fragment.data(), geometry.data());
	}

	waterShader_.load("Shaders/water.vs", "Shaders/water.fs");

	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	width_ = vp[2];
	height_ = vp[3];

}


World::~World(){

}

void World::addTerrain(int x, int z, size_t len, size_t col) {
	//allTerrain_[x].emplace(std::make_pair(z, Terrain(x*size_, z*size_, size_ / 2.0f, len, col, &shader_, &waterShader_, &heightGenerator_)));
	Terrain tempoTerrain(x*size_, z*size_, size_ / 2.0f, len, col, &shader_, &waterShader_, &heightGenerator_);
	allTerrain_[x].emplace(std::make_pair(z, std::move(tempoTerrain)));
}

void World::addTerrainThread(int x, int z, size_t len, size_t col) {
	//allTerrain_[x].emplace(std::make_pair(z, Terrain(x*size_, z*size_, size_ / 2.0f, len, col, &shader_, &waterShader_, &heightGenerator_, false)));
	Terrain tempoTerrain(x*size_, z*size_, size_ / 2.0f, len, col, &shader_, &waterShader_, &heightGenerator_);
	{
		std::lock_guard<std::mutex> guard(editDataMutex);
		allTerrain_[x].emplace(std::make_pair(z, std::move(tempoTerrain)));
	}

	toActivate.emplace_back(x, z);
	std::cout << "Fin thread \n";
}

float World::getY(float x, float z) {

	float copyX(x), copyZ(z);

	x > 0 ? x += size_ / 2.f : x -= size_ / 2.f;
	z > 0 ? z += size_ / 2.f : z -= size_ / 2.f;

	int xIndex(x / size_), zIndex(z / size_);
	{
		std::lock_guard<std::mutex> guard(editDataMutex);
		if (allTerrain_.find(xIndex) != allTerrain_.end()) {
			if (allTerrain_[xIndex].find(zIndex) != allTerrain_[xIndex].end()) {
				return allTerrain_[xIndex].at(zIndex).getY(copyX, copyZ);
			}
		}
	}

	return 1;
}

void World::terrainCheckThread(size_t nbChunk) {

	std::lock_guard<std::mutex> guard(checkTerrainMutex);

	//std::cout << "New check terrain thread\n";

	std::vector<int> checkPos;

	float x(camera_->Position.x), z(camera_->Position.z);

	x > 0 ? x += size_ / 2.f : x -= size_ / 2.f;
	z > 0 ? z += size_ / 2.f : z -= size_ / 2.f;

	int xInt(x / size_), zInt(z / size_);

	checkPos.push_back(xInt);
	checkPos.push_back(zInt);

	for (size_t j(1); j < nbChunk; ++j) {
		checkPos.push_back(xInt + j);
		checkPos.push_back(zInt);

		checkPos.push_back(xInt - j);
		checkPos.push_back(zInt);

		checkPos.push_back(xInt + j);
		checkPos.push_back(zInt);

		checkPos.push_back(xInt - j);
		checkPos.push_back(zInt);
	}

	for (size_t i(1); i < nbChunk; ++i) {

		checkPos.push_back(xInt);
		checkPos.push_back(zInt + i);

		checkPos.push_back(xInt);
		checkPos.push_back(zInt + i);

		checkPos.push_back(xInt);
		checkPos.push_back(zInt - i);

		checkPos.push_back(xInt);
		checkPos.push_back(zInt - i);


		for (size_t j(1); j < nbChunk - i; ++j) {
			checkPos.push_back(xInt + j);
			checkPos.push_back(zInt + i);

			checkPos.push_back(xInt - j);
			checkPos.push_back(zInt + i);

			checkPos.push_back(xInt + j);
			checkPos.push_back(zInt - i);

			checkPos.push_back(xInt - j);
			checkPos.push_back(zInt - i);
		}
	}

	/*checkPos.push_back(xInt + 1);
	checkPos.push_back(zInt);

	checkPos.push_back(xInt - 1);
	checkPos.push_back(zInt);

	checkPos.push_back(xInt);
	checkPos.push_back(zInt + 1);

	checkPos.push_back(xInt);
	checkPos.push_back(zInt - 1);*/



	for (size_t i(0), taille(checkPos.size() / 2); i < taille; ++i) {

		int xIndex(checkPos[i * 2]), zIndex(checkPos[i * 2 + 1]);

		//std::cout << "Cordonnée monde : X : " << xIndex << " Z : " << zIndex << std::endl;
		{
			std::lock_guard<std::mutex> guard(editDataMutex);

			if (allTerrain_.find(xIndex) != allTerrain_.end()) {
				if (allTerrain_[xIndex].find(zIndex) != allTerrain_[xIndex].end()) {
					//std::cout << "ancien monde trouvé\n";
					continue;
				}
			}
		}

		if (reservedTerrain_.find(xIndex) != reservedTerrain_.end()) {
			if (reservedTerrain_[xIndex].find(zIndex) != reservedTerrain_[xIndex].end()) {
				continue;
			}
		}

		//std::cout << "nouveau monde" << std::endl;
		//myWorld.addTerrain(j, i, 64, 64);
		//allTerrain_[xIndex].emplace(std::make_pair(zIndex, Terrain(xIndex*size_, zIndex*size_, size_ / 2.0f, 64, 64, &shader_, &waterShader_, &heightGenerator_)));
		std::cout << "X : " << xIndex << " Z : " << zIndex << std::endl;
		std::cout << "Debut thread\n";

		reservedTerrain_[xIndex].insert(zIndex);

		std::thread newThread(&World::addTerrainThread, this, xIndex, zIndex, 64, 64);

		newThread.detach();

	}
}

void World::update(size_t nbChunk) {

	if (checkTerrainMutex.try_lock()) {
		std::thread newThread(&World::terrainCheckThread, this, nbChunk);
		newThread.detach();
		checkTerrainMutex.unlock();
	}

	//Activation des nouveau terrain prêt 
	for (size_t i(0); i < toActivate.size(); ++i) {

		{
			std::lock_guard<std::mutex> guard(editDataMutex);

			std::cout << "X : " << toActivate[i].first << " ? : " << (allTerrain_.find(toActivate[i].first) != allTerrain_.end()) << " Z : " << toActivate[i].second << " ? : " << (allTerrain_[toActivate[i].first].find(toActivate[i].second) != allTerrain_[toActivate[i].first].end()) << std::endl;
			allTerrain_[toActivate[i].first].at(toActivate[i].second).sendData();

			toActivate[i] = std::move(toActivate.back());

			toActivate.pop_back();

			--i;

		}



	}

	//a ajouter : mutex pour créer le terrain en safe

}


void World::draw() {

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	shader_.use();


	//camera/view transformation
	//shader_.setMat4("view", camera.GetViewMatrix());


	//Uniform

	shader_.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
	shader_.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f); // assombri un peu la lumière pour correspondre à la scène
	shader_.setVec3("light.specular", 0.5f, 0.5f, 0.5f);

	shader_.setVec3("light.direction", 0.f, -1.0f, 0.0f);

	shader_.setFloat("light.constant", 1.0f);
	shader_.setFloat("light.linear", 0.09f);
	shader_.setFloat("light.quadratic", 0.032f);

	shader_.setFloat("material.shininess", 32.0f);

	//

	shader_.setMat4("projection", glm::perspective(glm::radians(camera_->Zoom), (float)width_ / (float)height_, 0.1f, 1000.0f));

	shader_.setMat4("view", camera_->GetViewMatrix());

	shader_.setVec3("viewPos", camera_->Position);



	shader_.setVec3("light.position", glm::vec3(0, 2, 0));

	shader_.setVec3("material.ambient", 0.0f, 0.58f, 0.25f);
	shader_.setVec3("material.diffuse", 0.0f, 0.58f, 0.25f);
	shader_.setVec3("material.specular", 0.0f, 0.7f, 0.25f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texGrass_);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texRock_);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texSnow_);

	shader_.setInt("texGrass", 0);
	shader_.setInt("texRock", 1);
	shader_.setInt("texSnow", 2);

	waterShader_.use();

	waterShader_.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
	waterShader_.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f); // assombri un peu la lumière pour correspondre à la scène
	waterShader_.setVec3("light.specular", 0.5f, 0.5f, 0.5f);

	waterShader_.setVec3("light.direction", 0.f, -1.0f, 0.0f);

	waterShader_.setFloat("light.constant", 1.0f);
	waterShader_.setFloat("light.linear", 0.09f);
	waterShader_.setFloat("light.quadratic", 0.032f);

	waterShader_.setFloat("material.shininess", 32.0f);

	//

	waterShader_.setMat4("projection", glm::perspective(glm::radians(camera_->Zoom), (float)width_ / (float)height_, 0.1f, 1000.0f));

	waterShader_.setMat4("view", camera_->GetViewMatrix());

	waterShader_.setVec3("viewPos", camera_->Position);



	waterShader_.setVec3("light.position", glm::vec3(0, 2, 0));

	waterShader_.setVec3("material.ambient", 0.33f, 0.78f, 1.0f);
	waterShader_.setVec3("material.diffuse", 0.33f, 0.78f, 1.0f);
	waterShader_.setVec3("material.specular", 1.0f, 1.0f, 1.0f);

	{
		std::lock_guard<std::mutex> guard(editDataMutex);

		for (auto it(allTerrain_.begin()), itEnd(allTerrain_.end()); it != itEnd; ++it) {
			for (auto it2(it->second.begin()), itEnd2(it->second.end()); it2 != itEnd2; ++it2) {
				it2->second.draw();
			}
		}
		for (auto it(allTerrain_.begin()), itEnd(allTerrain_.end()); it != itEnd; ++it) {
			for (auto it2(it->second.begin()), itEnd2(it->second.end()); it2 != itEnd2; ++it2) {
				it2->second.drawWater();
			}
		}
	}


	/*for (size_t i(0), taille(allTerrain_.size()); i < taille; ++i) {
		allTerrain_[i].draw();
	}*/
}