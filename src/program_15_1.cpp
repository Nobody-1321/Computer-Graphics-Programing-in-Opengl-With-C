#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <Utils.hpp>
using namespace std;

#define numVAOs 1
#define numVBOs 3

float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }
void window_size_callback(GLFWwindow* win, int newWidth, int newHeight);


Utils util = Utils();

GLuint renderingProgram, renderingProgramCubeMap;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

// variable allocation for display
GLuint vLoc, mvLoc, projLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;
GLuint skyboxTexture;
float cameraHeight = 2.0f, cameraPitch = 15.0f;
float planeHeight = 0.0f;

void setupVertices(void) {
	float cubeVertexPositions[108] =
	{ -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f
	};

	float PLANE_POSITIONS [18] = {
		-128.0f, 0.0f, -128.0f,  -128.0f, 0.0f, 128.0f,  128.0f, 0.0f, -128.0f,
		128.0f, 0.0f, -128.0f,  -128.0f, 0.0f, 128.0f,  128.0f, 0.0f, 128.0f
	};
	
	float PLANE_TEXCOORDS[12] = {
		0.0f, 0.0f,  0.0f, 1.0f,  1.0f, 0.0f,
		1.0f, 0.0f,  0.0f, 1.0f,  1.0f, 1.0f
	};

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexPositions), cubeVertexPositions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(PLANE_POSITIONS), PLANE_POSITIONS, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(PLANE_TEXCOORDS), PLANE_TEXCOORDS, GL_STATIC_DRAW);
}

void init(GLFWwindow* window) {
	renderingProgram = Utils::createShaderProgram("./shaders/vertex_shader151.glsl", "./shaders/fragment_shader151.glsl");
	if (Utils::checkOpenGLError()) {
		std::cerr << "ERROR: Could not create the shader program" << std::endl;
	}
	renderingProgramCubeMap = Utils::createShaderProgram("./shaders/vertex_shader151C.glsl", "./shaders/fragment_shader151C.glsl");
	if (Utils::checkOpenGLError()) {
		std::cerr << "ERROR: Could not create the shader program" << std::endl;
	}
	setupVertices();


	skyboxTexture = Utils::loadCubeMap("./textures/cubeMap3"); // expects a folder name
	if (Utils::checkOpenGLError()) {
		std::cerr << "ERROR: Could not load the cube map" << std::endl;
	}

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void display(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	glfwGetFramebufferSize(window, &width, &height);

	aspect = (float)width / (float)height;
	//pMat = glm::perspective(toRadians(60.0), aspect, 0.1f, 1000.0f);
	pMat = glm::perspective(toRadians(90.0), aspect, 0.1f, 1000.0f);

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -cameraHeight, 0.0f))
		* glm::rotate(glm::mat4(1.0f), toRadians(cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f));

	// draw cube map

	glUseProgram(renderingProgramCubeMap);

	vLoc = glGetUniformLocation(renderingProgramCubeMap, "v_matrix");
	projLoc = glGetUniformLocation(renderingProgramCubeMap, "p_matrix");

	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);	// cube is CW, but we are viewing the inside
	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glEnable(GL_DEPTH_TEST);

	// draw scene
	glUseProgram(renderingProgram);

	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, planeHeight, 0.0f));
	mvMat = vMat * mMat;

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

int main(void) {
    if (!glfwInit()) {
        std::cerr << "ERROR: GLFW could not initialize" << std::endl;
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, " program_15_1 ", nullptr, nullptr);    
    
	if (!window) {
        std::cerr << "ERROR: GLFW window could not be created" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "ERROR: GLAD could not initialize" << std::endl;
        return -1;
    }

    glfwSwapInterval(1);
	
	glfwSetWindowSizeCallback(window, window_size_callback);

    init(window);

    while (!glfwWindowShouldClose(window)) {
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(toRadians(90.0), aspect, 0.1f, 1000.0f);
}
