#include <Core.hpp>
#include <iostream>
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

    int select = 0;
    int colorRange[6] = {0, 255, 0, 255, 0, 255};
    bool showDemoWindow = false;
    auto LoggingLevel = spdlog::level::err;

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
    fboProgram.SetUniform1i("screenTexture", 0);

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
        ImGui::Begin("Options");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::Begin("Conversion Buttons");
        ImGui::RadioButton("RGB", &select, 0);
        ImGui::RadioButton("BGR", &select, 1);
        ImGui::RadioButton("Grayscale", &select, 2);
        ImGui::RadioButton("HSV", &select, 3);
        ImGui::End();

        ImGui::Begin("Color Masking");
        ImGui::DragIntRange2("Hue", &colorRange[0], &colorRange[1], 1.0f, 0, 255, "%d");
        ImGui::DragIntRange2("Saturation", &colorRange[2], &colorRange[3], 1.0f, 0, 255, "%d");
        ImGui::DragIntRange2("Value", &colorRange[4], &colorRange[5], 1.0f, 0, 255, "%d");

        ImGui::End();
        window.ImGuiRenderFrame();

        fbo.Bind();

        renderer.Clear();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        shaderProgram.Bind();
        texture.Bind();
        renderer.Draw(vao, ibo, shaderProgram);

        // Bind the default framebuffer
        fbo.Unbind();
        // Draw the framebuffer rectangle
        fboProgram.Bind();
        fboProgram.SetUniform1i("texCodeId", select);
        fboProgram.SetUniform1f("hmin", static_cast<float>(colorRange[0]) / 255);
        fboProgram.SetUniform1f("hmax", static_cast<float>(colorRange[1]) / 255);
        fboProgram.SetUniform1f("smin", static_cast<float>(colorRange[2]) / 255);
        fboProgram.SetUniform1f("smax", static_cast<float>(colorRange[3]) / 255);
        fboProgram.SetUniform1f("vmin", static_cast<float>(colorRange[4]) / 255);
        fboProgram.SetUniform1f("vmax", static_cast<float>(colorRange[5]) / 255);

        fboVAO.Bind();
        fbo.BindTexture();
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    vao.Unbind();
    ibo.Unbind();
    shaderProgram.Unbind();
    return 0;
}