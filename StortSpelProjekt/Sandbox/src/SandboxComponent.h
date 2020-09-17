#ifndef SANDBOXCOMPONENT_H
#define SANDBOXCOMPONENT_H

#include "..\ECS\Components\Component.h"
#include <string>

namespace component
{
    class SandboxComponent : public Component
    {
    public:
        SandboxComponent(Entity* parent);
        virtual ~SandboxComponent();

        void Update(double dt);

        std::string GetSandbox();

    private:
        std::string m_Sandbox;
    };
}

#endif