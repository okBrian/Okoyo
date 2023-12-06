#include <Core.hpp>
#include <iostream>
using namespace Core;

#define width 1280
#define height 720
// std::inrange
class App : public Core::CoreApp
{
private:
    // Everything needed to make a Square on the screen;
    Renderer renderer;
    WindowsPlatform window;
    VAO vao;
    IBO ibo;
    Shader shaderProgram;
    Shader fboProgram;
    Texture texture;

    FBO fbo;
    VAO fboVAO;
    GLfloat vertices[16] = {
        //     Position       TexCoord
        -1.0f, 1.0f, 1.0f, 1.0f,  // top left
        1.0f, 1.0f, 0.0f, 1.0f,   // top right
        -1.0f, -1.0f, 1.0f, 0.0f, // below left
        1.0f, -1.0f, 0.0f, 0.0f   // below right
    };
    // Set up index
    GLuint indices[6] = {
        0, 1, 2,
        1, 2, 3};

    float fboVertices[24] =
        {
            // Coords    // texCoords
            1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 1.0f,

            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 1.0f};

    int select;
    int colorRange[6] = {0, 255, 0, 255, 0, 255};
    bool showDemoWindow = false;

public:
    App()
        : CoreApp(), window(WindowsPlatform("Okoyo Image Proccessing", width, height)), select(0)
    {
        // Load OpenGL, Enable GL Debugging, and VSync - True //
        if (!LoadOpenGLContext())
        {
            window.Close();
            return;
        }
        EnableOpenGLDebug();
        window.setVSync(true);

        // Stuff for Base Image //
        VBO vbo;
        vao.init();
        vbo.init(vertices, sizeof(vertices), GL_STATIC_DRAW);
        ibo.init(indices, sizeof(indices));
        shaderProgram.init("../../res/default.vert", "../../res/default.frag");
        ImGuiClass::init(Window::getWindow());

        Layout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        vao.AddBuffer(vbo, layout);

        shaderProgram.Bind();
        texture.init("../../res/Assets/wow.jpg", 0, GL_RGB, GL_RGB);
        texture.Bind();
        texture.texUnit(shaderProgram, "tex0", 0);

        vao.Unbind();
        vbo.Unbind();
        ibo.Unbind();
        shaderProgram.Unbind();

        // Framebuffer Crap //
        VBO fboVBO;
        fboVAO.init();
        fboVBO.init(fboVertices, sizeof(fboVertices), GL_STATIC_DRAW);

        Layout fboLayout;
        fboLayout.Push<float>(2);
        fboLayout.Push<float>(2);
        fboVAO.AddBuffer(fboVBO, fboLayout);

        fboVAO.Unbind();
        fboVBO.Unbind();

        fbo.init(width, height);
        fbo.Bind();
        fbo.AttachColorBuffer();

        RBO rbo;
        rbo.init(width, height);
        rbo.AttachRBO();

        fboProgram.init("../../res/framebuffer.vert", "../../res/framebuffer.frag");
        fboProgram.Bind();
        glUniform1i(glGetUniformLocation(fboProgram.getID(), "screenTexture"), 0);

        // Error checking framebuffer //
        auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
            APP_ERROR("Framebuffer error: {}", fboStatus);
    }

    ~App()
    {
        vao.Unbind();
        ibo.Unbind();
        shaderProgram.Unbind();
        ImGuiClass::shutdown();
    }

    void onUpdate() override
    {
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

    void onImGuiRenderer() override
    {

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
    }
};

Core::CoreApp *Core::CreateApp()
{
    return new App();
}