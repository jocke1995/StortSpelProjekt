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
    // To use a skybox:
    // 1. Create skyboxcompoenent on an entity
    // 2. Set Mesh
    // 3. Set Cubemap Texture
    class SkyboxComponent : public Component
    {
    public:
        SkyboxComponent(Entity* parent);
        virtual ~SkyboxComponent();

        void RenderUpdate(double dt);

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
