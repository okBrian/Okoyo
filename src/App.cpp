#include <Core.hpp>
using namespace Core;

class App : public Core::CoreApp
{
private:
    Renderer renderer;
    WindowsPlatform window; 
    float vertices[24]
    {
        // Coords    // texCoords
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,

         1.0f,  1.0f,  1.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };
public:
    App()
        : CoreApp(), window(WindowsPlatform()), renderer(Renderer())
    {
        window.setVSync(true);

        
    }

    ~App() 
    {
    }

    void run()
    {
        while(window.WindowRunning)
        {
            window.onUpdate();
        }
    }
};

Core::CoreApp* Core::CreateApp()
{
    return new App();
}