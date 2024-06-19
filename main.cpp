#include <iostream>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <Windows.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h""

using namespace std;

#define SCREENW 800
#define SCREENH 600
#define PI 3.14

#define CIRCLERESOLUTION 100

class Shader
{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use()
    {
        glUseProgram(ID);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREENW, SCREENH, "walk_module", NULL, NULL);
	glfwMakeContextCurrent(window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glViewport(0, 0, SCREENW, SCREENH);

    float crown[] = {
        0.0f, 0.0f,   0.0f,
        -0.3f, 0.3f,  0.0f,
        -0.3f, 1.f,   0.0f,
        0.0f, 0.3f,   0.0f,
        0.3f, 1.f,    0.0f,
        0.3f, 0.3f,   0.0f,
        1.0f, 0.3f,   0.0f,
        0.3f, 0.0f,   0.0f,
        1.0f, -0.3f,  0.0f,
        0.3f, -0.3f,  0.0f,
        0.3f, -1.0f,  0.0f,
        0.0f, -0.3f,  0.0f,
        -0.3f, -1.0f, 0.0f,
        -0.3, -0.3f,  0.0f,
        -1.0f, -0.3f, 0.0f,
        -0.3f, 0.0f,  0.0f,
        -1.0f, 0.3f,  0.0f,
        -0.3f, 0.3f,  0.0f
    };


    float circle[CIRCLERESOLUTION * 3];
    for (int i = 0; i < CIRCLERESOLUTION*3; i+=3) {
        circle[i] = cos((PI * 2 / CIRCLERESOLUTION) * i) * 0.3;
        circle[i + 1] = sin((PI * 2 / CIRCLERESOLUTION) * i) * 0.3;
        circle[i + 2] = 1.0f;
    }



    //glEnableVertexAttribArray(0);
    /*
    unsigned int innerTriangleVAO;
    glGenVertexArrays(1, &innerTriangleVAO);
    unsigned int innerTriangleVBO;
    glGenBuffers(1, &innerTriangleVBO);
    glBindVertexArray(innerTriangleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, innerTriangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(innerTriangles), innerTriangles, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(float)*2));
    glEnableVertexAttribArray(1);
    */

    unsigned int crownVAO;
    unsigned int crownVBO;
    glGenVertexArrays(1, &crownVAO);
    glGenBuffers(1, &crownVBO);

    glBindVertexArray(crownVAO);
    glBindBuffer(GL_ARRAY_BUFFER, crownVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crown), crown, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(float) * 2));
    glEnableVertexAttribArray(1);

    unsigned int circleVAO;
    unsigned int circleVBO;
    glGenVertexArrays(1, &circleVAO);
    glGenBuffers(1, &circleVBO);

    glBindVertexArray(circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(float) * 2));
    glEnableVertexAttribArray(1);

    Shader mainShader("vertex.vs", "fragment.fs");
    mainShader.use();


    /*
    glm::mat4 projection(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)SCREENW / (float)SCREENH, 0.1f, 100.0f);

    glm::mat4 view(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.0f));
    */

    //mainShader.setMat4("projection", projection);
    //mainShader.setMat4("view", view);
    
    glm::mat4 transformMatrix(1.0f);

    mainShader.setInt("ScreenSizeX", SCREENW);

    float centrePosX = 0;
    float centrePosY = 0;
    float angle = 0;
    float positionAngle = 0;

    float speed = 0.01; // angular speed around the half-circle
    float rotationSpeed = 0.01; // around the centre of the figure

    int direction = 1;
    int rotationDirection = 1;
    float frequency = 1;

    float shapeSize = 0.05;

    int cooldown = 0;

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    bool isMoving = false;

	while (true) {
		glfwPollEvents();

        if (glfwWindowShouldClose(window)) {
            return 0;
        }

        if ((positionAngle >= PI || positionAngle < 0) && isMoving == true) {
            direction *= -1;
            rotationDirection *= -1;

            cooldown = frequency / 16 * 1000;
            isMoving = false;
            //cout << "direction changed";
        }

        if (cooldown > 0 && isMoving == false) {
            cooldown--;
            //cout << cooldown << "\n";
        }
        else isMoving = true;
        
        if (isMoving) {
            positionAngle += speed * direction;
            centrePosX = cos(positionAngle) * (1 - shapeSize * (float)SCREENH / (float)SCREENW);
            centrePosY = sin(positionAngle) - 1.0f;
            angle += rotationSpeed * rotationDirection;

            //cout << "x = " << centrePosX << " y = " << centrePosY << " positionAngle = " << positionAngle << endl;
        }

        transformMatrix = glm::mat4(1.0f);
        transformMatrix = glm::translate(transformMatrix, glm::vec3(centrePosX, centrePosY, 0));
        transformMatrix = glm::scale(transformMatrix, glm::vec3((float)SCREENH/ (float)SCREENW, 1, 1));
        transformMatrix = glm::rotate(transformMatrix, angle, glm::vec3(0, 0, 1));
        transformMatrix = glm::scale(transformMatrix, glm::vec3(shapeSize, shapeSize, 1));

        mainShader.setMat4("transform", transformMatrix);

		glClearColor(0.2, 0.3, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glBindVertexArray(crownVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 18);
        glBindVertexArray(circleVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)CIRCLERESOLUTION);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("##1");

            ImGui::SliderFloat("shape size", &shapeSize, 0.0001, 1);
            ImGui::SliderFloat("speed ", &speed, 0.00001, 0.5);
            ImGui::SliderFloat("frequency ", &frequency, 1, 20);
            ImGui::SliderFloat("rotation speed ", &rotationSpeed, 0, 0.1);
            if (ImGui::Button("reset Position")) {
                centrePosX = -1 + shapeSize;
                centrePosY = -1 + shapeSize;
                isMoving = false;
            }
            if (ImGui::Button("begin movement")) {
                isMoving = true;
            }
            if (ImGui::Button("resetSize")) {
                shapeSize = 0.05f;
            }
            if (ImGui::Button("wireframe mode")) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }


            ImGui::End();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
        Sleep(16);
    }

}