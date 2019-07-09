// Jeu_GL.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include "pch.h"

#include "head.h"

#include "Shader.hpp"
#include "Camera.hpp"

#include "World.h"

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <ctime>

//GLM
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include <vector>

//Fonctions
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);


const GLint WIDTH = 1600, HEIGHT = 900;

//Variable global pour la camera
static Camera camera(glm::vec3(0.0f, 2.0f, 0.0f));
static float lastX = WIDTH / 2.0f;
static float lastY = HEIGHT / 2.0f;
static bool firstMouse = true;

// timing
static float deltaTime = 0.0f;	// time between current frame and last frame
static float lastFrame = 0.0f;



int main() {

	srand(time(NULL));

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Test OpenGL", nullptr, nullptr);

	int screenWidth, screenHeight;

	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	if (window == nullptr) {
		std::cerr << "Impossible de créer l'ecran GLFW." << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;

	if (GLEW_OK != glewInit()) {
		std::cout << "Failes to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	glViewport(0, 0, screenWidth, screenHeight);

	//RAJOUTER LE SHADERS
	//Shader ourShader("Shaders/vertex.vs", "Shaders/fragment.fs");
	



	//Initialisation terminé

	/*unsigned int VAO(0), VBO(0);
	std::vector<float> myMesh;

	genMesh(10, 10, myMesh);
	
	createVAO(VAO, VBO, myMesh, { 3, 2 });*/

	//Texture
	unsigned int tex = loadTexture("Textures/grass.jpg");


	World myWorld(100, "Shaders/vertex.vs", "Shaders/fragment.fs", "Shaders/geometry.gs", &camera, tex);

	std::cout << "Premier \n";
	//myWorld.addTerrain(0, 0, 128, 128);
	/*myWorld.addTerrain(0, 1, 128, 128);
	myWorld.addTerrain(0, 2, 128, 128);
	myWorld.addTerrain(0, 3, 128, 128);
	myWorld.addTerrain(1, 3, 128, 128);
	myWorld.addTerrain(2, 3, 128, 128);
	myWorld.addTerrain(1, 2, 128, 128);*/
	std::cout << "Second \n";
	//myWorld.addTerrain(-1, 0, 10, 10);
	//myWorld.addTerrain(2, 0, 10, 10);
	//myWorld.addTerrain(3, 0, 10, 10);
	//myWorld.addTerrain(-2, 0, 10, 10);
	//myWorld.addTerrain(0, 1, 10, 10);
	//smyWorld.addTerrain(0, -1, 10, 10);
	//myWorld.addTerrain(1, -1, 10, 10);
	//myWorld.addTerrain(-1, -1, 10, 10);
	//myWorld.addTerrain(0, 2, 10, 10);
	//myWorld.addTerrain(0, 3, 10, 10);

	
	for (int i(-5); i < 5; ++i) {
		for (int j(-5); j < 5; ++j) {
			myWorld.addTerrain(j, i, 64, 124);
		}
	}
	

	/*ourShader.use();

	ourShader.setMat4("projection", glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f));*/

	//camera/view transformation
	//ourShader.setMat4("view", camera.GetViewMatrix());


	//Uniform
	/*ourShader.use();

	ourShader.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
	ourShader.setVec3("light.diffuse", 1.f, 1.f, 1.f); // assombri un peu la lumière pour correspondre à la scène
	ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

	ourShader.setVec3("light.direction", 0.f, -1.0f, 0.f);

	ourShader.setFloat("light.constant", 1.0f);
	ourShader.setFloat("light.linear", 0.09f);
	ourShader.setFloat("light.quadratic", 0.032f);

	ourShader.setFloat("material.shininess", 32.0f);*/




	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		//Check for events/input
		glfwPollEvents();

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);


		//Handle game logic

		camera.Position.y += -9.1 * deltaTime;
		//std::cout << "Pos X : " << camera.Position.x << " Pos Z : " << camera.Position.z << std::endl;
		float tempoY(myWorld.getY(camera.Position.x, camera.Position.z) + 1);
		if (tempoY > camera.Position.y) camera.Position.y = tempoY;
		camera.Position.y = 70.0f;
		//Render
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Draw OpenGL

		myWorld.update(5);
		myWorld.draw();



		glfwSwapBuffers(window);

	}

	std::cout << "Hello World!\n";

	return EXIT_SUCCESS;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

}


// Exécuter le programme : Ctrl+F5 ou menu Déboguer > Exécuter sans débogage
// Déboguer le programme : F5 ou menu Déboguer > Démarrer le débogage

// Conseils pour bien démarrer : 
//   1. Utilisez la fenêtre Explorateur de solutions pour ajouter des fichiers et les gérer.
//   2. Utilisez la fenêtre Team Explorer pour vous connecter au contrôle de code source.
//   3. Utilisez la fenêtre Sortie pour voir la sortie de la génération et d'autres messages.
//   4. Utilisez la fenêtre Liste d'erreurs pour voir les erreurs.
//   5. Accédez à Projet > Ajouter un nouvel élément pour créer des fichiers de code, ou à Projet > Ajouter un élément existant pour ajouter des fichiers de code existants au projet.
//   6. Pour rouvrir ce projet plus tard, accédez à Fichier > Ouvrir > Projet et sélectionnez le fichier .sln.
