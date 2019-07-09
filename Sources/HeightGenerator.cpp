#include "pch.h"
#include "HeightGenerator.h"
#include <iostream>


HeightGenerator::HeightGenerator() : seed_(rand()%1000000000), amplitude_(70.0f) {

	mountainTerrain.SetSeed(seed_);
	normalTerrain.SetSeed(seed_);
	baseColline.SetSeed(seed_);
	choosePlaineColline.SetSeed(seed_);
	choosePlaineMont.SetSeed(seed_);
	baseFlatTerrain.SetSeed(seed_);
	terrainType.SetSeed(seed_);


	//Montagne
	mountainTerrain.SetOctaveCount(30);
	mountainTerrain.SetFrequency(0.01);
	//mountainTerrain.SetPersistence(0.25);

	scaleMontain.SetSourceModule(0, mountainTerrain);
	scaleMontain.SetScale(1);
	scaleMontain.SetBias(0.75);

	//Plaine
	normalTerrain.SetOctaveCount(3);
	normalTerrain.SetFrequency(0.01);
	normalTerrain.SetPersistence(0.25);
	normalTerrain.SetLacunarity(3);

	/*
	testCurve.SetSourceModule(0, normalTerrain);
	testCurve.AddControlPoint(-0.7, -0.4);
	testCurve.AddControlPoint(0.4, 0);
	*/

	scaleNormal.SetSourceModule(0, normalTerrain);
	scaleNormal.SetScale(0.3);
	scaleNormal.SetBias(-0.2);

	baseColline.SetOctaveCount(2);
	baseColline.SetFrequency(0.01);
	baseColline.SetPersistence(0.3);
	baseColline.SetLacunarity(2);

	scaleColline.SetSourceModule(0, baseColline);
	scaleColline.SetScale(0.3);
	scaleColline.SetBias(0.10);

	//Pour choisir le type entre plaine et Colline (Couple 3 )
	choosePlaineColline.SetOctaveCount(1);
	choosePlaineColline.SetFrequency(0.002);
	//choosePlaineColline.SetPersistence(0.4);
	//choosePlaineColline.SetLacunarity(2);

	//Pour fusioner les terrain en fonction du perlin
	//Selection
	plaineCollineSelector.SetSourceModule(0, scaleNormal);
	plaineCollineSelector.SetSourceModule(1, scaleColline);
	plaineCollineSelector.SetControlModule(choosePlaineColline);
	plaineCollineSelector.SetBounds(0, 1000.0);
	plaineCollineSelector.SetEdgeFalloff(0.125);


	///Fin Couple 3

	choosePlaineMont.SetFrequency(0.00035);
	terrainType.SetPersistence(0.25);

	//Selection
	plaineMontSelector.SetSourceModule(0, plaineCollineSelector);
	plaineMontSelector.SetSourceModule(1, scaleMontain);
	plaineMontSelector.SetControlModule(choosePlaineMont);
	plaineMontSelector.SetBounds(0.25, 1000.0);
	plaineMontSelector.SetEdgeFalloff(0.25);

	//Fin setUp couple 2



	baseFlatTerrain.SetFrequency(0.005);
	//myModule.SetPersistence(0.4);

	flatTerrain.SetSourceModule(0, baseFlatTerrain);
	flatTerrain.SetScale(0.125);
	flatTerrain.SetBias(-1);

	//Perlin
	//terrainType.SetOctaveCount(1);
	terrainType.SetFrequency(0.0035);
	terrainType.SetPersistence(0.25);


	//Selection
	terrainSelector.SetSourceModule(0, flatTerrain);
	terrainSelector.SetSourceModule(1, plaineMontSelector);
	terrainSelector.SetControlModule(terrainType);
	terrainSelector.SetBounds(0, 1000.0);
	terrainSelector.SetEdgeFalloff(0.250);


	terrainScaler.SetSourceModule(0, terrainSelector);
	terrainScaler.SetScale(amplitude_);
	terrainScaler.SetBias(amplitude_);


	finalTerrain.SetSourceModule(0, terrainScaler);
	finalTerrain.SetFrequency(4);
	finalTerrain.SetPower(0.125);




}


HeightGenerator::~HeightGenerator(){
}

float HeightGenerator::generateHeight(float x, float z) {
	//int octave = 3;
	//float roughness = 0.9f;

	//float total(0);
	//float d = (float)pow(2, octave - 1);
	/*for (int i = 0; i < octave; i++) {
		float freq = (float)(pow(2, i) / (d*20.0f));
		float amp = (float)pow(roughness, i) * amplitude_;
		total += getInterpolatedNoise((x)*freq, (z)*freq) * amp;
	}*/

	/*float freqMax = 0.0325;
	float ampDep = 3;*/
	

	/*for (int i = 0; i < octave; i++) {
		float amp = amplitude_/(float)pow(ampDep, i);
		float tempo = getInterpolatedNoise((x)*freqMax, (z)*freqMax) * amp;
		total += tempo;
		freqMax /= 2.0f;
	}

	float e = 0.8f;

	if (total < 0) {
		total = -powf(-total, e);
	}
	else {
		total = powf(total, e);
	}*/

	/*float e0 = 1 * ridgenoise(x/16.0, z/16.0);
	float e1 = 0.5 * ridgenoise(x/8.0, z/8.0) * e0;
	float e2 = 0.25 * ridgenoise(x/4.0, z/4.0) * (e0 + e1);
	float e = e0 + e1 + e2;
	return pow(e, 1);*/

	//return ridgenoise(x / 4.0, z / 4.0) * amplitude_;


	float total = terrainScaler.GetValue(x, z, 0);

	//std::cout << total << std::endl;

	return total;


	//return total;
}

float HeightGenerator::getSmoothNoise(int x, int z) {
	float side2((noise_2d(x - 2, z) + noise_2d(x + 2, z) + noise_2d(x, z - 2) + noise_2d(x, z + 2)) / 16.0f);
	float corner((noise_2d(x - 1, z - 1) + noise_2d(x + 1, z - 1) + noise_2d(x - 1, z + 1) + noise_2d(x + 1, z + 1))/16.0f);
	float side((noise_2d(x - 1, z) + noise_2d(x + 1, z) + noise_2d(x, z - 1) + noise_2d(x, z + 1))/8.0f);
	float center(noise_2d(x, z)/4.00f);
	return corner + side + center +side2;
}

float HeightGenerator::ridgenoise(float x, float z) {
	return 2 * (0.5 - abs(0.5 - getInterpolatedNoise(x, z)));
}

float HeightGenerator::getInterpolatedNoise(float x, float z) {
	int xInt(floor(x)), zInt(floor(z));

	float xFrac(x - xInt), zFrac(z - zInt);


	float v1(getSmoothNoise(xInt, zInt)), v2(getSmoothNoise(xInt + 1, zInt)), v3(getSmoothNoise(xInt, zInt + 1)), v4(getSmoothNoise(xInt + 1, zInt + 1));

	float i1(interpolate(v1, v2, xFrac)), i2(interpolate(v3, v4, xFrac));

	//std::cout << "Partie entiere : xInt : " << xInt << " zInt : " << zInt << " y Obetenue : " << interpolate(i1, i2, zFrac) << std::endl;

	return interpolate(i1, i2, zFrac);
}

float HeightGenerator::interpolate(float a, float b, float blend) {
	float pi(3.141592);
	float theta(pi * blend);
	float f((1.0f - cos(theta)) * 0.5f);
	//std::cout << " f obtenue : " << f << std::endl;
	return a * (1.0f - f) + b * f;
	//return a * 0.5 + b * 0.5;
}

float HeightGenerator::getNoise(int x, int z) {
	srand(x * 3345489 + z * 255489 + seed_);
	return (rand()/(float)RAND_MAX)*2.0f - 1.0f;
}

double HeightGenerator::rand_noise(int t){
	t = (t << 13) ^ t;
	t = (t * (t * t * 15731 + 789221) + 1376312589);
	return 1.0 - (t & 0x7fffffff) / 1073741824.0;
}

double HeightGenerator::noise_2d(int x, int z){
	int tmp = rand_noise(x) * 850000;
	return rand_noise(tmp + z);
}
