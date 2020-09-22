#ifndef SKYBOXCOMPONENT_H
#define SKYBOXCOMPONENT_H

#include "Component.h"
#include "structs.h"

class Mesh;
class Model;
class Texture;
class Transform;
class BaseCamera;

namespace component
{
    class SkyboxComponent : public Component
    {
    public:
        SkyboxComponent(Entity* parent);
        virtual ~SkyboxComponent();

        void Update(double dt);

        // Sets
        void SetMesh(Mesh* mesh);
        void SetTexture(Texture* texture);
        void SetCamera(BaseCamera* camera);

        // Gets
        Transform* GetTransform() const;
        Mesh* GetMesh() const;
        Texture* GetTexture() const;

    private:
        Mesh* m_Mesh = nullptr;
        Texture* m_Texture = nullptr;
        Transform* m_pTransform = nullptr;

        // Always set pos to cameras
        BaseCamera* m_Camera = nullptr;
    };
}
#endif
