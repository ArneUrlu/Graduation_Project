#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <imgui\imgui.h>
#include <imgui\imgui_impl_opengl3.h>
#include <imgui\imgui_impl_glfw.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdio.h>
#include <vector>
#include <cmath>

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Renderer.h"
#include "Texture.h"


int main()
{
    // Konstante
    const int clothWidth = 10;
    const int clothHeight = 10;
    const int sphereSubdivision = 20;
    const float pI = 3.1416f;

    // Sphere geometry
    std::vector<float> sphereVertices;
    std::vector<unsigned int> sphereIndices;

    // Generate sphere vertices and indices
    for (int lat = 0; lat <= sphereSubdivision; ++lat) {
        float theta = lat * pI / sphereSubdivision;
        float sinTheta = sinf(theta);
        float cosTheta = cosf(theta);

        for (int lon = 0; lon <= sphereSubdivision; ++lon) {
            float phi = lon * 2 * pI / sphereSubdivision;
            float sinPhi = sinf(phi);
            float cosPhi = cosf(phi);

            float x = cosPhi * sinTheta;
            float y = cosTheta;
            float z = sinPhi * sinTheta;

            sphereVertices.push_back(x);
            sphereVertices.push_back(y);
            sphereVertices.push_back(z);
        }
    }
    
    // Generate sphere indices
    for (int lat = 0; lat < sphereSubdivision; ++lat) {
        for (int lon = 0; lon < sphereSubdivision; ++lon) {
            int currRow = lat * (sphereSubdivision + 1);
            int nextRow = (lat + 1) * (sphereSubdivision + 1);

            sphereIndices.push_back(currRow + lon);
            sphereIndices.push_back(nextRow + lon);
            sphereIndices.push_back(currRow + lon + 1);

            sphereIndices.push_back(currRow + lon + 1);
            sphereIndices.push_back(nextRow + lon);
            sphereIndices.push_back(nextRow + lon + 1);
        }
    }

    std::vector<float> clothVertices;
    std::vector<unsigned int> clothIndices;

    // Generate cloth vertices
    for (int row = 0; row < clothHeight; ++row) {
        for (int col = 0; col < clothWidth; ++col) {
            float x = static_cast<float>(col) / (clothWidth - 1);
            float y = 1.0f;
            float z = static_cast<float>(row) / (clothHeight - 1);

            clothVertices.push_back(x);
            clothVertices.push_back(y);
            clothVertices.push_back(z);
        }
    }

    // Generate cloth indices
    for (int row = 0; row < clothHeight - 1; ++row) {
        for (int col = 0; col < clothWidth - 1; ++col) {
            int currVertex = row * clothWidth + col;
            int nextRowVertex = (row + 1) * clothWidth + col;

            clothIndices.push_back(currVertex);
            clothIndices.push_back(nextRowVertex);
            clothIndices.push_back(currVertex + 1);

            clothIndices.push_back(currVertex + 1);
            clothIndices.push_back(nextRowVertex);
            clothIndices.push_back(nextRowVertex + 1);
        }
    }


    //Inicijalizacija GLFW za window u OpenGL kontekstu
    if (!glfwInit())
    {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //GLFW prozor za prikaz
    GLFWwindow* window = glfwCreateWindow(800, 800, "AI27-2014 Cloth Simulation", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    //Inicijalizacija GLAD	zajedno sa exepion handling

    if (!gladLoadGL())
    {
        // Greska u inicijalizaciji
        return -1;
    }

    // Inicijalizacija ImGUI
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = static_cast<float>(800);
    io.DisplaySize.y = static_cast<float>(800);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    VertexBuffer sphereVertexBuffer(sphereVertices.data(), sphereVertices.size() * sizeof(float));
    IndexBuffer sphereIndexBuffer(sphereIndices.data(), sphereIndices.size());

    VertexBuffer clothVertexBuffer(clothVertices.data(), clothVertices.size() * sizeof(float));
    IndexBuffer clothIndexBuffer(clothIndices.data(), clothIndices.size());

    // Vertex Array Objects and Layouts
    VertexArray sphereVAO;
    VertexBufferLayout sphereLayout;
    sphereLayout.push<float>(3);
    sphereVAO.AddBuffer(sphereVertexBuffer, sphereLayout);

    VertexArray clothVAO;
    VertexBufferLayout clothLayout;
    clothLayout.push<float>(3);
    clothVAO.AddBuffer(clothVertexBuffer, clothLayout);

    // Shaders

    Shader sphereShader("src/Basic.shader");
    Shader clothShader("src/Basic.shader");

    // SETUP ALL MATRICES FOR DRAWING IN 3D

    glm::mat4 projectionMatrix = glm::perspective(glm::radians(60.0f), 800.0f / 800.0f, 0.1f, 150.0f);

    glm::vec3 cameraPosition = glm::vec3(4.5f, 2.0f, 4.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.6f, 0.0f);
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);

    glm::vec3 objectPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 objectRotation = glm::vec3(0.0f, glm::radians(90.0f), 0.0f);
    glm::vec3 objectScale = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::mat4 modelMatrix = glm::mat4(1.0f); // Identity matrix
    modelMatrix = glm::translate(modelMatrix, objectPosition);
    modelMatrix = glm::rotate(modelMatrix, objectRotation.y, glm::vec3(0, 1, 0));
    modelMatrix = glm::scale(modelMatrix, objectScale);

    glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll events

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        // Create ImGui interface for cloth simulation settings
        ImGui::SetNextWindowPos(ImVec2(10, 610), ImGuiCond_Always);
        ImGui::Begin("Cloth Simulation Settings");

        //Slider-i za cloth elasticnost i tezinu
        static float stiffness = 100.0f;
        static float damping = 0.5f;
        ImGui::SliderFloat("Stiffness", &stiffness, 0.0f, 500.0f);
        ImGui::SliderFloat("Damping", &damping, 0.0f, 1.0f);

        //Start Dugme
        if (ImGui::Button("Start"))
        {
            //Handle Button click
        }
        ImGui::SameLine();
        //Reset Dugme
        if (ImGui::Button("Reset"))
        {
            //Handle Button Click
        }
        ImGui::End();

        // Clear the screen
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Render the sphere
        sphereShader.bind();
        sphereShader.SetUniform4f("u_Color", 0.25f, 0.21f, 0.8f, 1.0f);
        sphereShader.SetUniformMat4("u_MVP", mvpMatrix);
        sphereVAO.bind();
        sphereIndexBuffer.bind();
        glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, nullptr);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        sphereShader.SetUniform4f("u_Color", 0.1f, 0.21f, 0.8f, 1.0f);
        glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, nullptr);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Push Matrix translate
        objectPosition = glm::vec3(0.0f, 2.0f, 0.0f);
        mvpMatrix = projectionMatrix * viewMatrix * glm::translate(glm::mat4(1.0f), objectPosition);
        

        
        // Render the cloth
        clothShader.bind();
        clothShader.SetUniform4f("u_Color", 0.5f, 0.21f, 0.5f, 1.0f); 
        clothShader.SetUniformMat4("u_MVP", mvpMatrix);
        clothVAO.bind();
        clothIndexBuffer.bind();
        glDrawElements(GL_TRIANGLES, clothIndices.size(), GL_UNSIGNED_INT, nullptr);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        clothShader.SetUniform4f("u_Color", 0.3f, 0.21f, 0.5f, 1.0f);
        glDrawElements(GL_TRIANGLES, clothIndices.size(), GL_UNSIGNED_INT, nullptr);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Pop Matrix
        objectPosition = glm::vec3(0.0f, 0.0f, 0.0f);
        mvpMatrix = projectionMatrix * viewMatrix * glm::translate(glm::mat4(1.0f), objectPosition);
        
        

        // Apply spring forces

        // Collision detection and resolution

        // Render particles and springs

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        sphereIndexBuffer.unbind();
        sphereVAO.unbind();
        sphereShader.unbind();

        clothIndexBuffer.unbind();
        clothVAO.unbind();
        clothShader.unbind();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Shutdown ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

    clothShader.~Shader();
    sphereShader.~Shader();

    // Cleanup and exit
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}