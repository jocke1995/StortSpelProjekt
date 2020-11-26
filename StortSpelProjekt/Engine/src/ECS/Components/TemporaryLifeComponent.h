#ifndef TEMPORARYLIFECOMPONENT_H
#define TEMPORARYLIFECOMPONENT_H

#include "Component.h"
#include "structs.h"

namespace component
{
    class TemporaryLifeComponent : public Component
    {
    public:
        TemporaryLifeComponent(Entity* parent, double duration);
        virtual ~TemporaryLifeComponent();

        void RenderUpdate(double dt);
        void OnInitScene();
        void OnUnInitScene();

    private:
        double m_Duration;
    };
}
#endif
