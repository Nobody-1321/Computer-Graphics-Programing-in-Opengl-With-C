#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "../private/stlHeaders.hpp"
#include "../../../libs/soild2/SOIL2/SOIL2.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Utils
{
private:
	static std::string readShaderFile(const char *filePath);
	static void printShaderLog(GLuint shader);
	static void printProgramLog(int prog);
	static GLuint prepareShader(int shaderTYPE, const char *shaderPath);
	static int finalizeShaderProgram(GLuint sprogram);

public:
	Utils();
	static bool checkOpenGLError();
	static bool isExtensionSupported(const char* extName);
	static GLuint createShaderProgramCP(const char *cp);
	static GLuint createShaderProgram(const char *vp, const char *fp);
	static GLuint createShaderProgram(const char *vp, const char *gp, const char *fp);
	static GLuint createShaderProgram(const char *vp, const char *tCS, const char* tES, const char *fp);
	static GLuint createShaderProgram(const char *vp, const char *tCS, const char* tES, char *gp, const char *fp);
	static GLuint loadTexture(const char *texImagePath);
	static GLuint loadCubeMap(const char *mapDir);
	static void displayComputeShaderLimits();

	static float* goldAmbient();
	static float* goldDiffuse();
	static float* goldSpecular();
	static float goldShininess();

	static float* silverAmbient();
	static float* silverDiffuse();
	static float* silverSpecular();
	static float silverShininess();

	static float* bronzeAmbient();
	static float* bronzeDiffuse();
	static float* bronzeSpecular();
	static float bronzeShininess();
};