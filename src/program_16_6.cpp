#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Utils.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <cstdlib>
#include <cstring>

constexpr int WIDTH  = 1024;
constexpr int HEIGHT = 1024;

int windowWidth = WIDTH;
int windowHeight = HEIGHT;

GLuint screenTextureID;         // ID de la textura generada por el shader
unsigned char* screenTexture;   // Buffer temporal (opcional)

constexpr int numVAOs = 1;
constexpr int numVBOs = 2;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

GLuint screenQuadShader = 0;
GLuint raytraceComputeShader = 0;
GLuint brickTexture, earthTexture;

void init(GLFWwindow* window) {
    const float windowQuadVerts[] = {
        -1.0f,  1.0f, 0.3f,
        -1.0f, -1.0f, 0.3f,
         1.0f, -1.0f, 0.3f,
         1.0f, -1.0f, 0.3f,
         1.0f,  1.0f, 0.3f,
        -1.0f,  1.0f, 0.3f
    };

    const float windowQuadUVs[] = {
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    // Buffer auxiliar (opcional) para comprobar errores visuales
    screenTexture = static_cast<unsigned char*>(malloc(4 * WIDTH * HEIGHT));
    memset(screenTexture, 0, 4 * WIDTH * HEIGHT);

	
    // Color rosa por defecto si el shader falla
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            int index = (y * WIDTH + x) * 4;
            screenTexture[index + 0] = 250;
            screenTexture[index + 1] = 128;
            screenTexture[index + 2] = 255;
            screenTexture[index + 3] = 255;
        }
    }

    // Crear textura de salida
    glGenTextures(1, &screenTextureID);
    glBindTexture(GL_TEXTURE_2D, screenTextureID);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, WIDTH, HEIGHT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenTexture);

    // Configurar VAO y VBOs
    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);

    glGenBuffers(numVBOs, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(windowQuadVerts), windowQuadVerts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(windowQuadUVs), windowQuadUVs, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);  // Desvincular

    // Cargar shaders
    screenQuadShader = Utils::createShaderProgram("shaders/vertex_shader162_.glsl", "shaders/fragment_shader162_.glsl");
    raytraceComputeShader = Utils::createShaderProgramCP("shaders/compute_shader166.glsl");

    brickTexture = Utils::loadTexture("textures/brick1.jpg");
	earthTexture = Utils::loadTexture("textures/world.jpg");
}

void display(GLFWwindow* window, double currentTime) {
    // Ejecución del shader de cómputo
    glUseProgram(raytraceComputeShader);
    glBindImageTexture(0, screenTextureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, brickTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, earthTexture);
    
    glActiveTexture(GL_TEXTURE0);

    glUniform2i(glGetUniformLocation(raytraceComputeShader, "image_size"), WIDTH, HEIGHT);
    
    //glDispatchCompute(WIDTH, HEIGHT, 1);
    glDispatchCompute((WIDTH  + 15) / 16, (HEIGHT + 15) / 16, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    //glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // Dibujar textura en pantalla
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(screenQuadShader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenTextureID);

    glBindVertexArray(vao[0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void setWindowSizeCallback(GLFWwindow* win, int newWidth, int newHeight) {
    glViewport(0, 0, newWidth, newHeight);
}

void cleanup() {
    glDeleteTextures(1, &screenTextureID);
    glDeleteBuffers(numVBOs, vbo);
    glDeleteVertexArrays(1, vao);
    glDeleteProgram(screenQuadShader);
    glDeleteProgram(raytraceComputeShader);
    free(screenTexture);
}

// ==== FUNCIÓN PRINCIPAL ====
int main() {
    if (!glfwInit()) {
        std::cerr << "Error inicializando GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "program_16_4", nullptr, nullptr);
    if (!window) {
        std::cerr << "No se pudo crear la ventana GLFW\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "No se pudo inicializar GLAD\n";
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, setWindowSizeCallback);

    init(window);

    while (!glfwWindowShouldClose(window)) {
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
