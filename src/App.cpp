#include <Core.hpp>


class App : public Core::CoreApp
{
public:
    App()
        : CoreApp()
    {
        CoreApp::init();
    }

    ~App() 
    {
        CoreAppShutdown();
    }

    void run() 
    {
        while(!glfwWindowShouldClose(getWindow()))
        {
            glfwPollEvents();
            glfwSwapBuffers(getWindow());
        }
    }
};

Core::CoreApp* Core::CreateApp()
{
    return new App();
}