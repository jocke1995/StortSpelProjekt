#ifndef PARTICLEEMITTERCOMPONENT_H
#define PARTICLEEMITTERCOMPONENT_H

#include "Component.h"
#include "structs.h"

class Mesh;
class Transform;
class Texture2DGUI;

namespace component
{
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
        void SetTexture(Texture2DGUI* texture);

        // Gets
        Mesh* GetMesh() const;
        Texture2DGUI* GetTexture() const;

    private:
        Mesh* m_pMesh = nullptr;
        Texture2DGUI* m_pTexture = nullptr;
    };
}
#endif
