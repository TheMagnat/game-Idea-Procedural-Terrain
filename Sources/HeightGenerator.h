#pragma once

#include<cstdlib>

#include <noise/noise.h>


class HeightGenerator
{
public:
	HeightGenerator();
	~HeightGenerator();

	float generateHeight(float x, float z);

	float ridgenoise(float x, float z);
	float getInterpolatedNoise(float x, float z);

	float interpolate(float a, float b, float blend);

	float getSmoothNoise(int x, int z);

	float getNoise(int x, int z);

	double rand_noise(int t);

	double noise_2d(int x, int z);


private:
	int seed_;

	float amplitude_;

	///Couple 1
	//Terre
		///Couple 2
		//Generer des montagnes
		noise::module::RidgedMulti mountainTerrain;
		noise::module::ScaleBias scaleMontain;


			///Couple 3
			//Plaine
			noise::module::Perlin normalTerrain;
			noise::module::ScaleBias scaleNormal;
			//noise::module::Curve testCurve;
			
			noise::module::Perlin  baseColline;
			noise::module::ScaleBias scaleColline;



			///Fin Couple 3

			//Pour choisir le type entre plaine et Colline (Couple 3 )
			noise::module::Perlin choosePlaineColline;

			//Pour fusioner les terrain en fonction du perlin
			noise::module::Select plaineCollineSelector;

		///FIN COUPLE 2


		//Pour choisir le type entre plaine et Montagne (Couple 2 )
		noise::module::Perlin choosePlaineMont;

		//Pour fusioner les terrain en fonction du perlin
		noise::module::Select plaineMontSelector;




	//Mers
	noise::module::Billow  baseFlatTerrain;
	noise::module::ScaleBias flatTerrain;
	///Fin Couple 1

	//Pour choisir le type entre terre et mers (Couple 1 )
	noise::module::Perlin terrainType;

	//Pour fusioner les terrain en fonction du perlin
	noise::module::Select terrainSelector;
	
	//Scale le final
	noise::module::ScaleBias terrainScaler;

	noise::module::Turbulence finalTerrain;


};

