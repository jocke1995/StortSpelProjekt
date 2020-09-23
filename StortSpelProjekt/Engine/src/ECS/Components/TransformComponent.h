#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include "Component.h"
#include <assimp\types.h>
class Transform;

namespace component
{
    class TransformComponent : public Component
    {
    public:
        TransformComponent(Entity* parent);
        virtual ~TransformComponent();

        void Update(double dt);
        void RenderUpdate(double dt);
        void Init(Renderer* renderer);

        Transform* GetTransform() const;
    private:
        Transform* m_pTransform = nullptr;
    };
}

#endif

