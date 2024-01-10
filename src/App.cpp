#include <Core.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <memory>
using namespace Core;

#define width 1280
#define height 720

int main()
{
    GLfloat vertices[16] =
    {
    //  Position       TexCoord
        -1.0f,  1.0f, 1.0f, 1.0f,  // top left
         1.0f,  1.0f, 0.0f, 1.0f,   // top right
        -1.0f, -1.0f, 1.0f, 0.0f, // below left
         1.0f, -1.0f, 0.0f, 0.0f   // below right
    };
    // Set up index
    GLuint indices[6] =
    {
        0, 1, 2,
        1, 2, 3
    };

    GLfloat fboVertices[24] =
    {
        // Coords    TexCoord
        1.0f, -1.0f, 1.0f, 0.0f,
       -1.0f, -1.0f, 0.0f, 0.0f,
       -1.0f,  1.0f, 0.0f, 1.0f,

        1.0f,  1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
       -1.0f,  1.0f, 0.0f, 1.0f
    };

    float floatKernel[9] = {
        1.0/16, 2.0/16, 1.0/16,
        2.0/16, 4.0/16, 2.0/16,
        1.0/16, 2.0/16, 1.0/16
    };
    float* floatKernelv = &floatKernel[0];

    bool showDemoWindow = true;
    auto LoggingLevel = spdlog::level::err;
    bool enableColorMaskRange = false; 
    bool enableKernel = false;
    bool enableColorConversions = true;

    int select = 0;
    ImVec4 colorMin = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 colorMax = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);


    Logging::init();
    Logging::setCoreLevel(&LoggingLevel);
    Window window("Okoyo", width, height);

    if (!LoadOpenGLContext())
    {
        window.Close();
        return 1;
    }

    EnableOpenGLDebug();
    window.setVSync(true);

    // Stuff for Base Image //
    Renderer renderer {};
    VAO vao {};
    VBO vbo (vertices, sizeof(vertices), GL_STATIC_DRAW);
    IBO ibo (indices, sizeof(indices));
    Shader shaderProgram ("../../res/default.vert", "../../res/default.frag");

    Layout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);
    vao.addBuffer(vbo, layout);

    shaderProgram.Bind();
    Texture texture ("../../res/Assets/wow.jpg", 0, GL_RGB, GL_RGB);
    texture.texUnit(shaderProgram, "tex0", 0);
    // texture.Unbind();

    vao.Unbind();
    vbo.Unbind();
    ibo.Unbind();
    shaderProgram.Unbind();

    // Framebuffer //
    VAO fboVAO {};
    VBO fboVBO (fboVertices, sizeof(fboVertices), GL_STATIC_DRAW);

    Layout fboLayout;
    fboLayout.Push<float>(2);
    fboLayout.Push<float>(2);
    fboVAO.addBuffer(fboVBO, fboLayout);

    fboVAO.Unbind();
    fboVBO.Unbind();

    FBO fbo (width, height);
    fbo.AttachColorBuffer();

    RBO rbo (width, height);
    rbo.AttachRBO();

    Shader fboProgram ("../../res/framebuffer.vert", "../../res/framebuffer.frag");
    fboProgram.Bind();
    fboProgram.setUniform1<GLint>("screenTexture", 0);

    // Error checking framebuffer //
    if (!fbo.isComplete())
        APP_ERROR("FRAMEBUFFER::ERROR:NOT COMPLETE");

    fbo.Unbind();
    fboProgram.Unbind();
    
    window.ImGuiInit();

    while(window.WindowRunning)
    {
        window.onUpdate();

        window.ImGuiNewFrame();
        ImGui::ShowDemoWindow(&showDemoWindow);
        ImGui::Begin("Option");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Checkbox("Coversion Buttons", &enableColorConversions);
        ImGui::Checkbox("Color Mask Range", &enableColorMaskRange);
        ImGui::Checkbox("Kernel", &enableKernel);
        ImGui::End();
        
        ImGui::Begin("Conversion Buttons");
        ImGui::RadioButton("RGB", &select, 0);
        ImGui::RadioButton("BGR", &select, 1);
        ImGui::RadioButton("Grayscale", &select, 2);
        ImGui::RadioButton("HSV", &select, 3);
        ImGui::End();

        ImGui::Begin("Color Mask Range");
        ImGui::Text("Color Min");
        ImGui::ColorEdit4("Color Min", (float*)&colorMin, ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_InputHSV);
        ImGui::Text("Color Max"); 
        ImGui::ColorEdit4("Color Max", (float*)&colorMax, ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_InputHSV);
        ImGui::End();
        
        ImGui::Begin("Kernel");
        ImGui::InputFloat3("1", floatKernelv);
        ImGui::InputFloat3("2", floatKernelv+3);
        ImGui::InputFloat3("3", floatKernelv+6);
        ImGui::End();

        fbo.Bind();

        renderer.Clear();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        shaderProgram.Bind();
        texture.Bind();
        renderer.Draw(vao, ibo, shaderProgram);
        // Bind the default framebuffer
        fbo.Unbind();
        // Draw the framebuffer rectangle
        shaderProgram.Unbind();
        fboProgram.Bind();
        fboProgram.setUniform1<GLint>("enableColorConversions", static_cast<int>(enableColorConversions));
        fboProgram.setUniform1<GLint>("enableColorMaskRange", static_cast<int>(enableColorMaskRange));
        fboProgram.setUniform1<GLint>("enableKernel", static_cast<int>(enableKernel));
        fboProgram.setUniform1<GLint>("texCodeId", select);
        fboProgram.setUniform2<GLfloat>("h", colorMin.x, colorMax.x);
        fboProgram.setUniform2<GLfloat>("s", colorMin.y, colorMax.y);
        fboProgram.setUniform2<GLfloat>("v", colorMin.z, colorMax.z);
        fboProgram.setUniformMatfv<glm::mat3>("kernel", glm::make_mat3(floatKernelv)); 

        fboVAO.Bind();
        fbo.BindTexture();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        fboProgram.Unbind();
        fboVAO.Unbind();

        window.ImGuiRenderFrame();
    }

    vao.Unbind();
    ibo.Unbind();
    shaderProgram.Unbind();
    return 0;
}