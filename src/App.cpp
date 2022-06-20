#include <Core.hpp>
using namespace Core;

class App : public Core::CoreApp
{
private:
    WindowsPlatform window; 
public:
    App()
        : CoreApp(), window(WindowsPlatform())
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