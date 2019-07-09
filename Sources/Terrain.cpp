#include "pch.h"
#include "Terrain.h"

#include <cmath>

#include <iostream>
#include<array>

static const float WATER_HEIGHT = 13;


/*
Terrain::Terrain(float x, float z, float size, size_t len, size_t col, Shader* shader, HeightGenerator* heightGenerator) : x_(x), z_(z), size_(size), len_(len), col_(col), nbVertices_(len*col*2*3), shader_(shader), heightGenerator_(heightGenerator) {

	//Nombre de carré (len * col), 2 triangles par carré, 3 points par triangle, 5 informations par point.
	data_.reserve(len*col * 2 * 3 * 5);
	heightMap_.resize((len + 1) * (col + 1));

	int xTest = (x / (size_*2.0f)) * (len_);
	int zTest = (z / (size_*2.0f)) * (col_);


	//On pourrait utiliser widthTexStep pour calculer widthStep et pareille pour height mais pour etre le plus précis possible sur les float je prefere le recalculer
	float widthStep(2.0f / len), heightStep(2.0f / col);
	float widthTexStep(1.0f / len), heightTexStep(1.0f / col);

	for (size_t i(0); i < col; ++i) {
		for (size_t j(0); j < len; ++j) {

			float actualZ((i / (float)col)*2.0f - 1.0f), actualX((j / (float)len)*2.0f - 1.0f);
			float actualYTex(i / (float)col), actualXTex(j / (float)len);

			glm::vec2 pos1((int)j + xTest, (int)i + zTest), pos2((int)j + 1 + xTest, (int)i + zTest), pos3((int)j + xTest, (int)i + 1 + zTest), pos4((int)j + 1 + xTest, (int)i + 1 + zTest);

			//float y1(heightGenerator_->generateHeight(actualX*size_ + x_, actualZ*size_ + z_)), y2(heightGenerator_->generateHeight((actualX + widthStep)*size_ + x_, actualZ*size_ + z_)), y3(heightGenerator_->generateHeight(actualX*size_ + x_, (actualZ + heightStep)*size_ + z_)), y4(heightGenerator_->generateHeight((actualX + widthStep)*size_ + x_, (actualZ + heightStep)*size_ + z_));
			float y1(heightGenerator_->generateHeight(pos1.x, pos1.y)), y2(heightGenerator_->generateHeight(pos2.x, pos2.y)), y3(heightGenerator_->generateHeight(pos3.x, pos3.y)), y4(heightGenerator_->generateHeight(pos4.x, pos4.y));

			//std::cout << "|| j : " << (int)j << " xTest : " << xTest << " Total = " << (int)j + xTest << " || i : " << i << " zTest : " << zTest << " Total = " << (int)i + zTest << " Y obtenue : " << y1 << std::endl;

			//std::cout << "Y1 : " << y1 << " Y2 : " << y2 << " Y3 : " << y3 << " Y4 : " << y4 << std::endl;

			//Premier Triangle
			//Premier Point
			//Position
			data_.push_back(actualX); //X
			data_.push_back(y1); //Y
			data_.push_back(actualZ); //Z
			heightMap_[i * (col + 1) + j] = y1; //Y dans la heightMap

			//Normal
			addNormal(pos1.x, pos1.y);

			//Texture
			data_.push_back(actualXTex); //TexX
			data_.push_back(actualYTex); //TexY


			//Second Point
			//Position
			data_.push_back(actualX + widthStep); //X
			data_.push_back(y2); //Y
			data_.push_back(actualZ); //Z

			//Normal
			addNormal(pos2.x, pos2.y);

			//Texture
			data_.push_back(actualXTex + widthTexStep); //TexX
			data_.push_back(actualYTex); //TexY


			//Troisieme point
			//Position
			data_.push_back(actualX); //X
			data_.push_back(y3); //Y
			data_.push_back(actualZ + heightStep); //Z
			heightMap_[(i+1) * (col + 1) + j] = y3; //Y dans la heightMap

			//Normal
			addNormal(pos3.x, pos3.y);

			//Texture
			data_.push_back(actualXTex); //TexX
			data_.push_back(actualYTex + heightTexStep); //TexY


			//Second Triangle
			//Premier Point
			//Position
			data_.push_back(actualX); //X
			data_.push_back(y3); //Y
			data_.push_back(actualZ + heightStep); //Z

			//Normal
			addNormal(pos3.x, pos3.y);

			//Texture
			data_.push_back(actualXTex); //TexX
			data_.push_back(actualYTex + heightTexStep); //TexY


			//Second Point
			//Position
			data_.push_back(actualX + widthStep); //X
			data_.push_back(y4); //Y
			data_.push_back(actualZ + heightStep); //Z
			heightMap_[(i + 1) * (col + 1) + j + 1] = y4; //Y dans la heightMap

			//Normal
			addNormal(pos4.x, pos4.y);

			//Texture
			data_.push_back(actualXTex + widthTexStep); //TexX
			data_.push_back(actualYTex + heightTexStep); //TexY


			//Troisieme point
			//Position
			data_.push_back(actualX + widthStep); //X
			data_.push_back(y2); //Y
			data_.push_back(actualZ); //Z
			heightMap_[i * (col + 1) + j + 1] = y2; //Y dans la heightMap

			//Normal
			addNormal(pos2.x, pos2.y);

			//Texture
			data_.push_back(actualXTex + widthTexStep); //TexX
			data_.push_back(actualYTex); //TexY
		}

	}

	createVAO(VAO_, VBO_, data_, { 3, 3, 2 });

}
*/

Terrain::Terrain(float x, float z, float size, size_t len, size_t col, Shader* shader, Shader* waterShader, HeightGenerator* heightGenerator, bool mainThread) : x_(x), z_(z), size_(size), len_(len), col_(col), nbVertices_(len*col*3), shader_(shader), waterShader_(waterShader), heightGenerator_(heightGenerator) {
	generateTerrain(mainThread);

	/*heightMap_.resize(6 * (len_ - 1)*(col_ - 1));

	for (size_t i(0); i < heightMap_.size(); ++i) {
		heightMap_[i] = 0;
	}*/
}

void Terrain::generateTerrain(bool mainThread) {
	size_t count = len_ * col_;

	position_.resize(count * 3);
	normal_.resize(count * 3);
	texPos_.resize(count * 2);

	heightMap_.resize(count);

	indices_.resize(6 * (len_ - 1)*(col_ - 1));

	int xTest = (x_ / (size_*2.0f)) * (len_ - 1);
	int zTest = (z_ / (size_*2.0f)) * (col_ - 1);

	isWater = false;

	size_t vertexPointer = 0;
	for (size_t i = 0; i < col_; i++) {
		for (size_t j = 0; j < len_; j++) {
			
			float tempoX((float)j * 2 / ((float)len_ - 1) - 1);
			float tempoZ((float)i * 2 / ((float)col_ - 1) - 1);

			//float tempoY(heightGenerator_->generateHeight((int)j + xTest, (int)i + zTest));
			float tempoY(heightGenerator_->generateHeight(tempoX*size_ + x_, tempoZ*size_ + z_));

			if (tempoY < WATER_HEIGHT) {
				isWater = true;
			}



			//Position
			position_[vertexPointer * 3] = tempoX;
			position_[vertexPointer * 3 + 1] = tempoY;
			position_[vertexPointer * 3 + 2] = tempoZ;


			heightMap_[i*len_ + j] = tempoY;
			//std::cout << "Position : X : " << (float)j / ((float)len_ - 1)<< " Z : " << (float)i / ((float)col_ - 1)<< std::endl;

			//Normals
			//glm::vec3 tempoNormal(getNormal(j, i));

			/*normal_[vertexPointer * 3] = tempoNormal.x;
			normal_[vertexPointer * 3 + 1] = tempoNormal.y;
			normal_[vertexPointer * 3 + 2] = tempoNormal.z;*/

			/*normal_[vertexPointer * 3] = 0;
			normal_[vertexPointer * 3 + 1] = 1;
			normal_[vertexPointer * 3 + 2] = 0;*/

			//Texture
			texPos_[vertexPointer * 2] = (float)j / ((float)len_ - 1);
			texPos_[vertexPointer * 2 + 1] = (float)i / ((float)col_ - 1);

			++vertexPointer;
		}
	}

	size_t pointer = 0;
	for (size_t gz = 0; gz < col_ - 1; ++gz) {
		for (size_t gx = 0; gx < len_ - 1; ++gx) {

			int topLeft = (gz*len_) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1)*len_) + gx;
			int bottomRight = bottomLeft + 1;

			indices_[pointer++] = topLeft;
			indices_[pointer++] = bottomLeft;
			indices_[pointer++] = topRight;
			indices_[pointer++] = topRight;
			indices_[pointer++] = bottomLeft;
			indices_[pointer++] = bottomRight;
		}
	}

	calculateNormals();

	//return loader.loadToVAO(vertices, textureCoords, normals, indices);
	//createVAO(VAO_, VBO_, data_, { 3, 3, 2 });

	if (isWater) {
		generateWater();
	}

	if (mainThread){
		sendData();
	}

}

void Terrain::generateWater() {
	size_t count = len_ * col_;

	waterPosition_.resize(count * 3);
	waterNormal_.resize(count * 3);
	waterTexPos_.resize(count * 2);

	waterIndices_.resize(6 * (len_ - 1)*(col_ - 1));

	size_t vertexPointer = 0;
	for (size_t i = 0; i < col_; i++) {
		for (size_t j = 0; j < len_; j++) {

			float tempoX((float)j * 2 / ((float)len_ - 1) - 1);
			float tempoZ((float)i * 2 / ((float)col_ - 1) - 1);


			//Position
			waterPosition_[vertexPointer * 3] = tempoX;
			waterPosition_[vertexPointer * 3 + 1] = 0;
			waterPosition_[vertexPointer * 3 + 2] = tempoZ;


			//heightMap_[i*len_ + j] = tempoY;

			waterNormal_[vertexPointer * 3] = 0;
			waterNormal_[vertexPointer * 3 + 1] = 1;
			waterNormal_[vertexPointer * 3 + 2] = 0;

			//Texture
			waterTexPos_[vertexPointer * 2] = (float)j / ((float)len_ - 1);
			waterTexPos_[vertexPointer * 2 + 1] = (float)i / ((float)col_ - 1);

			++vertexPointer;
		}
	}

	size_t pointer = 0;
	for (size_t gz = 0; gz < col_ - 1; ++gz) {
		for (size_t gx = 0; gx < len_ - 1; ++gx) {

			int topLeft = (gz*len_) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1)*len_) + gx;
			int bottomRight = bottomLeft + 1;

			waterIndices_[pointer++] = topLeft;
			waterIndices_[pointer++] = bottomLeft;
			waterIndices_[pointer++] = topRight;
			waterIndices_[pointer++] = topRight;
			waterIndices_[pointer++] = bottomLeft;
			waterIndices_[pointer++] = bottomRight;
		}
	}

}


void Terrain::sendData(){

	createVAO(VAO_, VBO_, VBO2_, VBO3_, EBO_, position_, normal_, texPos_, indices_);

	if (isWater) {
		createVAO(waterVAO_, waterVBO_, waterVBO2_, waterVBO3_, waterEBO_, waterPosition_, waterNormal_, waterTexPos_, waterIndices_);
	}
}

void Terrain::calculateNormals() {
	
	size_t vertexPointer = 0;
	for (size_t i = 0; i < col_; i++) {
		for (size_t j = 0; j < len_; j++) {

			//std::cout << "Point X = " << j << " Y = " << i << std::endl;

			size_t p1((i - 1)*len_ + j), p2(i*len_ + j);

			std::array<int, 6> faceIndex;
			std::array<glm::vec3, 6> faceNormal;

			faceIndex[0] = ((p1 - (i - 1)) * 2 - 1);
			faceIndex[1] = (faceIndex[0] + 1);
			faceIndex[2] = (faceIndex[1] + 1);
			faceIndex[3] = ((p2 - i) * 2 - 2);
			faceIndex[4] = (faceIndex[3] + 1);
			faceIndex[5] = (faceIndex[4] + 1);




			if (i == 0) {
				faceIndex[0] = -1;
				faceNormal[0] = calculateNormal(j, i - 1, j - 1, i, j, i);

				faceIndex[1] = -1;
				faceNormal[1] = calculateNormal(j, i - 1, j, i, j + 1, i - 1);


				faceIndex[2] = -1;
				faceNormal[2] = calculateNormal(j + 1, i - 1, j, i, j + 1, i);

			}
			else if (i == col_ - 1) {
				faceIndex[3] = -1;
				faceNormal[3] = calculateNormal(j - 1, i, j - 1, i + 1, j, i);

				faceIndex[4] = -1;
				faceNormal[4] = calculateNormal(j, i, j - 1, i + 1, j, i + 1);

				faceIndex[5] = -1;
				faceNormal[5] = calculateNormal(j, i, j, i + 1, j + 1, i);

			}

			if (j == 0) {
				faceIndex[0] = -1;
				faceNormal[0] = calculateNormal(j, i - 1, j - 1, i, j, i);

				faceIndex[3] = -1;
				faceNormal[3] = calculateNormal(j - 1, i, j - 1, i + 1, j, i);

				faceIndex[4] = -1;
				faceNormal[4] = calculateNormal(j, i, j - 1, i + 1, j, i + 1);

			}
			else if (j == len_ - 1) {
				faceIndex[1] = -1;
				faceNormal[1] = calculateNormal(j, i - 1, j, i, j + 1, i - 1);

				faceIndex[2] = -1;
				faceNormal[2] = calculateNormal(j + 1, i - 1, j, i, j + 1, i);

				faceIndex[5] = -1;
				faceNormal[5] = calculateNormal(j, i, j, i + 1, j + 1, i);

			}
			
			for (size_t i2(0); i2 < 6; ++i2) {
				//std::cout << "i : " << i << " j : " << j << std::endl;
				//std::cout << "i2 : " << i2 << " faceIndex[i2] : " << faceIndex[i2] << std::endl;
				if (faceIndex[i2] == -1) {
					//faceNormal[i2] = glm::vec3(0.0f);
					continue;
				}
				else {
					faceNormal[i2] = calculateNormal(faceIndex[i2]);
				}
			}

			glm::vec3 normal(glm::normalize(faceNormal[0] + faceNormal[1] + faceNormal[2] + faceNormal[3] + faceNormal[4] + faceNormal[5]));

			normal_[vertexPointer * 3] = normal.x;
			normal_[vertexPointer * 3 + 1] = normal.y;
			normal_[vertexPointer * 3 + 2] = normal.z;

			//std::cout << "Normal : X =  " << normal.x << " Y = " << normal.y << " Z = " << normal.z << std::endl;

			++vertexPointer;
		}
	}

}

glm::vec3 Terrain::calculateNormal(int x1, int z1, int x2, int z2, int x3, int z3){

	/*int xTest = (x_ / (size_*2.0f)) * (len_ - 1);
	int zTest = (z_ / (size_*2.0f)) * (col_ - 1);*/

	//Test
	//float tempoY(heightGenerator_->generateHeight((int)x + xTest, (int)z + zTest));

	//Position

	float tempoX1((float)x1 * 2 / ((float)len_ - 1) - 1), tempoZ1((float)z1 * 2 / ((float)col_ - 1) - 1);
	float tempoX2((float)x2 * 2 / ((float)len_ - 1) - 1), tempoZ2((float)z2 * 2 / ((float)col_ - 1) - 1);
	float tempoX3((float)x3 * 2 / ((float)len_ - 1) - 1), tempoZ3((float)z3 * 2 / ((float)col_ - 1) - 1);

	glm::vec3 p1((float)x1 * 2 / ((float)len_ - 1) - 1, heightGenerator_->generateHeight(tempoX1 * size_ + x_, tempoZ1 * size_ + z_), (float)z1 * 2 / ((float)col_ - 1) - 1);
	glm::vec3 p2((float)x2 * 2 / ((float)len_ - 1) - 1, heightGenerator_->generateHeight(tempoX2 * size_ + x_, tempoZ2 * size_ + z_), (float)z2 * 2 / ((float)col_ - 1) - 1);
	glm::vec3 p3((float)x3 * 2 / ((float)len_ - 1) - 1, heightGenerator_->generateHeight(tempoX3 * size_ + x_, tempoZ3 * size_ + z_), (float)z3 * 2 / ((float)col_ - 1) - 1);

	/*glm::vec3 p1((float)x1 * 2 / ((float)len_ - 1) - 1, heightGenerator_->generateHeight((int)x1 + xTest, (int)z1 + zTest), (float)z1 * 2 / ((float)col_ - 1) - 1);
	glm::vec3 p2((float)x2 * 2 / ((float)len_ - 1) - 1, heightGenerator_->generateHeight((int)x2 + xTest, (int)z2 + zTest), (float)z2 * 2 / ((float)col_ - 1) - 1);
	glm::vec3 p3((float)x3 * 2 / ((float)len_ - 1) - 1, heightGenerator_->generateHeight((int)x3 + xTest, (int)z3 + zTest), (float)z3 * 2 / ((float)col_ - 1) - 1);*/

	/*position_[vertexPointer * 3] = (float)x * 2 / ((float)len_ - 1) - 1;
	position_[vertexPointer * 3 + 1] = tempoY;
	position_[vertexPointer * 3 + 2] = (float)z * 2 / ((float)col_ - 1) - 1;*/
	
	return glm::normalize(glm::cross(p2 - p1, p3 - p1));

}

glm::vec3 Terrain::calculateNormal(int face){


	//std::cout << "taille indice : " << indices_.size() << std::endl;

	//std::cout << "face : " << face << " *3 = " << face * 3 << std::endl;

	face *= 3;


	size_t a(indices_[face]), b(indices_[face+1]), c(indices_[face+2]);

	//std::cout << "a : " << a << " b : " << b << " c : " << c << std::endl;

	float* tempo = &position_[a*3];
	glm::vec3 p1(position_[a * 3], position_[a * 3 + 1], position_[a * 3 + 2]);

	tempo = &position_[b*3];
	glm::vec3 p2(position_[b * 3], position_[b * 3 + 1], position_[b * 3 + 2]);

	tempo = &position_[c*3];
	glm::vec3 p3(position_[c * 3], position_[c * 3 + 1], position_[c * 3 + 2]);

	//std::cout << "indice A : " << a << " B : " << b << " C : " << c << std::endl;
	/*std::cout << "via position \n\n";
	std::cout << "P1 x : " << position_[a*3] << " y : " << position_[a*3 + 1] << " z : " << position_[a*3 + 2] << std::endl;
	std::cout << "P2 x : " << position_[b*3] << " y : " << position_[b*3 + 1] << " z : " << position_[b*3 + 2] << std::endl;
	std::cout << "P3 x : " << position_[c*3] << " y : " << position_[c*3 + 1] << " z : " << position_[c*3 + 2] << std::endl; 
		std::cout << "Via PT MTN \n\n";
	std::cout << "P1 x : " << p1.x << " y : " << p1.y << " z : " << p1.z << std::endl;
	std::cout << "P2 x : " << p2.x << " y : " << p2.y << " z : " << p2.z << std::endl;
	std::cout << "P3 x : " << p3.x << " y : " << p3.y << " z : " << p3.z << std::endl;*/

	return glm::normalize(glm::cross(p2 - p1, p3 - p1));
}

void Terrain::addNormal(int x, int z) {
	float L(heightGenerator_->generateHeight(x - 1, z)), R(heightGenerator_->generateHeight(x + 1, z)), D(heightGenerator_->generateHeight(x, z - 1)), U(heightGenerator_->generateHeight(x, z + 1));
	//glm::vec3 normal(glm::normalize(glm::vec3(heightL - heightR, 2.0f, heightD - heightU)));
	//glm::vec3 normal(glm::normalize(0.25f*glm::vec3(2 * (R - L), 2 * (D - U), -4)));
	glm::vec3 normal(glm::normalize(glm::vec3(L - R, 2.0f, D - U)));

	data_.push_back(normal.x);
	data_.push_back(normal.y);
	data_.push_back(normal.z);



}

glm::vec3 Terrain::getNormal(int x, int z) {
	/*float L(heightGenerator_->generateHeight(x - 1, z)), R(heightGenerator_->generateHeight(x + 1, z)), D(heightGenerator_->generateHeight(x, z - 1)), U(heightGenerator_->generateHeight(x, z + 1));
	glm::vec3 normal(glm::normalize(glm::vec3(L - R, 2.0f, D - U)));

	return normal;*/


	float heightL = heightGenerator_->generateHeight(x - 1, z);
	float heightR = heightGenerator_->generateHeight(x + 1, z);
	float heightD = heightGenerator_->generateHeight(x, z - 1);
	float heightU = heightGenerator_->generateHeight(x, z + 1);
	glm::vec3 normal = glm::vec3(heightL - heightR, 2.0f, heightD - heightU);
	normal = glm::normalize(normal);
	return normal;

}

float Terrain::getY(float x, float z) {

	float xStep(1.0f / (len_ - 1));
	float zStep(1.0f / (col_ - 1));
	float moyStep((xStep + zStep) / 2.0f);

	x = ((x - x_)/(size_) + 1.0f)/2.0f;
	z = ((z - z_)/(size_) + 1.0f)/2.0f;

	size_t beforeX((len_-1) * x), beforeZ((col_-1) * z);

	
	//Si l'index de X ou Z choisi est le dernier prendre celui avant.
	if (x == 1.0f) beforeX -= 1;
	if (z == 1.0f) beforeZ -= 1;

	size_t afterX(beforeX + 1), afterZ(beforeZ + 1);

	//std::cout << "BeforeX : " << beforeX << " AfterX : " << afterX << " BeforeZ : " << beforeZ << " AfterZ : " << afterZ << std::endl;

	
	float moduloX(fmod(x, xStep)), moduloZ(fmod(z, zStep));

	glm::vec2 actualP(x, z), p1, p2, p3;
	float y1, y2, y3;

	if (moduloX + moduloZ < moyStep) {
		///Haut gauche
		//Before before
		//after before
		//before after

		//std::cout << "Y1 : " << beforeZ * (len_)+beforeX << " Y2 : " << beforeZ * (len_)+afterX << " Y3 : " << afterZ * (len_)+beforeX << std::endl;

		p1.x = beforeX / (float)(len_-1);
		p1.y = beforeZ / (float)(col_-1);
		y1 = heightMap_[beforeZ*(len_) + beforeX];

		p2.x = afterX / (float)(len_-1);
		p2.y = beforeZ / (float)(col_-1);
		y2 = heightMap_[beforeZ*(len_) + afterX];

		p3.x = beforeX / (float)(len_-1);
		p3.y = afterZ / (float)(col_-1);
		y3 = heightMap_[afterZ*(len_) + beforeX];

	}
	else {
		///Bad droit
		//after after
		//before after
		//after before

		//std::cout << "Y1 : " << afterZ * (len_)+afterX << " Y2 : " << afterZ * (len_)+beforeX << " Y3 : " << beforeZ * (len_)+afterX << std::endl;


		p1.x = afterX / (float)(len_-1);
		p1.y = afterZ / (float)(col_-1);
		y1 = heightMap_[afterZ*(len_) + afterX];


		p2.x = beforeX / (float)(len_-1);
		p2.y = afterZ / (float)(col_-1);
		y2 = heightMap_[afterZ*(len_) + beforeX];


		p3.x = afterX / (float)(len_-1);
		p3.y = beforeZ / (float)(col_-1);
		y3 = heightMap_[beforeZ*(len_) + afterX];

	}

	float dist1(glm::distance(actualP.x, p1.x)), dist2(glm::distance(actualP.y, p1.y));
	float vecY1(y2 - y1), vecY2(y3 - y1);

	dist1 = dist1 / glm::distance(p2.x, p1.x); dist2 = dist2 / glm::distance(p3.y, p1.y);


	float finalY(y1 + dist1*vecY1 + dist2*vecY2);

	return finalY;
}

void Terrain::draw() {

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	shader_->use();
	glm::mat4 model(glm::translate(glm::mat4(1.0f), glm::vec3(x_, 0, z_)));
	model = glm::scale(model, glm::vec3(size_, 1, size_));

	shader_->setMat4("model", model);
	//Pour adapter le model aux normal pour des normal qui suive le model
	shader_->setMat3("modelNormal", glm::mat3(glm::transpose(glm::inverse(model))));

	glBindVertexArray(VAO_);
	//glDrawArrays(GL_TRIANGLES, 0, nbVertices_);
	glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

}

void Terrain::drawWater() {

	if (!isWater) return;

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	waterShader_->use();

	glm::mat4 model(glm::translate(glm::mat4(1.0f), glm::vec3(x_, WATER_HEIGHT, z_)));
	model = glm::scale(model, glm::vec3(size_, 1, size_));

	shader_->setMat4("model", model);
	//Pour adapter le model aux normal pour des normal qui suive le model
	shader_->setMat3("modelNormal", glm::mat3(glm::transpose(glm::inverse(model))));

	glBindVertexArray(waterVAO_);
	glDrawElements(GL_TRIANGLES, waterIndices_.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

}