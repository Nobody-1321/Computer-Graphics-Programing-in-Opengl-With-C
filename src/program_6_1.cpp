#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Utils.hpp>
#include <Sphere.hpp>

using namespace std;

#define numVAOs 1
#define numVBOs 3  //changed from 2 to 3

float cameraX, cameraY, cameraZ;

GLuint renderingProgram;
GLuint worldTexture;

GLuint vao[numVAOs];
GLuint vbo[numVBOs];

GLuint projLoc, mvLoc;
int width, height;
float aspect;

glm::mat4 pMat, vMat, mMat, mvMat;
Sphere mySphere(1.0f, 72, 36);  // Initialize a sphere with radius 1.0, 72 sectors, and 36 stacks

void setupVertices() {
    // get the vertices and indices from sphere object
    std::vector<unsigned int> ind = mySphere.getIndices(); 
    std::vector<glm::vec3> vert = mySphere.getVertices();

    // create a vector to store the vertex data 
    std::vector<float> pvalues;
    for (const auto& v : vert) {
        pvalues.push_back(v.x);
        pvalues.push_back(v.y);
        pvalues.push_back(v.z);
    }

    int numIndices = mySphere.getNumIndices();
    int numVertices = mySphere.getNumVertices();

    glGenVertexArrays(numVAOs, vao);
    glBindVertexArray(vao[0]);

    glGenBuffers(numVBOs, vbo);

    // configure VBO for vertices
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, pvalues.size() * sizeof(float), &pvalues[0], GL_STATIC_DRAW);

    // Configure VBO for indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * sizeof(unsigned int), &ind[0], GL_STATIC_DRAW);

    // Configure VBO for texture coordinates
    std::vector<glm::vec2> texCoords = mySphere.getTexCoords();
    std::vector<float> tvalues;
    for (const auto& t : texCoords) {
        tvalues.push_back(1.0f - t.s); // <--- Invertir el eje X
        tvalues.push_back(t.t);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, tvalues.size() * sizeof(float), &tvalues[0], GL_STATIC_DRAW);

}

void init(GLFWwindow *window) {
    
    renderingProgram = Utils::createShaderProgram(
        "./shaders/vertex_shader61.glsl", "./shaders/fragment_shader61.glsl");

    if (Utils::checkOpenGLError()) {
        std::cerr << "ERROR: Could not create the shader program" << std::endl;
    }

    glfwGetFramebufferSize(window, &width, &height);
    aspect = static_cast<float>(width) / static_cast<float>(height);

    pMat = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);

    cameraX = 0.0f; cameraY = 0.0f; cameraZ = 4.0f;

    worldTexture = Utils::loadTexture("./textures/world.jpg");

    setupVertices();
}

void display(GLFWwindow *window, double currentTime) {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glClearColor(0.0, 0.0, 0.0, 1.0);

    glUseProgram(renderingProgram);

    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

    // Aplicar la rotación de 45 grados alrededor del eje Y
    float angle = glm::radians(100.0f);
    mMat = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));
    mMat = glm::rotate(mMat, (float)currentTime, glm::vec3(0.0f, 0.0f, 1.0f));
    

    mvMat = vMat * mMat;

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // link the vertex attributes with the buffer data
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0); // link the texture attributes with the buffer data
    glEnableVertexAttribArray(1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, worldTexture);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glDrawElements(GL_TRIANGLES, mySphere.getNumIndices(), GL_UNSIGNED_INT, 0);
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

    //GLFWwindow* window = glfwCreateWindow(1080, 720, "program_6_1", NULL, NULL);
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, " program_6_1 ", nullptr, nullptr);    
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
