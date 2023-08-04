#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <imgui\imgui.h>
#include <imgui\imgui_impl_opengl3.h>
#include <imgui\imgui_impl_glfw.h>
#include <stdio.h>

int main()
{
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
        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        

        // Clear the screen

        // Update particle positions

        // Apply spring forces

        // Collision detection and resolution

        // Render particles and springs

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Shutdown ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

    // Cleanup and exit
    glfwDestroyWindow(window);
    glfwTerminate();

	return 0;
}