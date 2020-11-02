#ifndef PARTICLEEMITTERCOMPONENT_H
#define PARTICLEEMITTERCOMPONENT_H

#include "Component.h"
#include "structs.h"

class Mesh;
class Model;
class TextureCubeMap;
class Transform;
class BaseCamera;

namespace component
{
    // To use a skybox:
    // 1. Create skyboxcompoenent on an entity
    // 2. Set Mesh
    // 3. Set Cubemap Texture
    class ParticleEmitterComponent : public Component
    {
    public:
        ParticleEmitterComponent(Entity* parent);
        virtual ~ParticleEmitterComponent();

        void RenderUpdate(double dt);
        void OnInitScene();
        void OnUnInitScene();

        // Sets
        void SetMesh(Mesh* mesh);
        void SetTexture(TextureCubeMap* texture);
        void SetCamera(BaseCamera* camera);

        // Gets
        Transform* GetTransform() const;
        Mesh* GetMesh() const;
        TextureCubeMap* GetTexture() const;
        BaseCamera* GetCamera() const;

    private:
        Mesh* m_pMesh = nullptr;
        TextureCubeMap* m_pTexture = nullptr;
    };
}
#endif
