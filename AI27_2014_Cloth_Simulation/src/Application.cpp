#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <imgui\imgui.h>
#include <imgui\imgui_impl_opengl3.h>
#include <imgui\imgui_impl_glfw.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>
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
            float y = static_cast<float>(row) / (clothHeight - 1);
            float z = 1.0f;

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
    GLFWwindow* window = glfwCreateWindow(1280, 720, "AI27-2014 Cloth Simulation", nullptr, nullptr);
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
    io.DisplaySize.x = static_cast<float>(1280);
    io.DisplaySize.y = static_cast<float>(720);

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

    Texture sphereTex("include/textures/sphereTexture.png");

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

        // Render the sphere
        sphereShader.bind();
        sphereShader.SetUniform4f("u_Color", 0.25f, 0.21f, 0.8f, 1.0f);
        sphereVAO.bind();
        sphereIndexBuffer.bind();
        glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, nullptr);
        
        

        // Render the cloth
        clothShader.bind();
        clothShader.SetUniform4f("u_Color", 0.5f, 0.21f, 0.5f, 1.0f);
        clothVAO.bind();
        clothIndexBuffer.bind();
        glDrawElements(GL_TRIANGLES, clothIndices.size(), GL_UNSIGNED_INT, nullptr);
        

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