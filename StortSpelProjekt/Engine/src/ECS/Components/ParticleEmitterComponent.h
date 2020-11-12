#ifndef PARTICLEEMITTERCOMPONENT_H
#define PARTICLEEMITTERCOMPONENT_H

#include "Component.h"
#include "structs.h"

#include "../Particles/ParticleEffect.h"

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
        void SetParticleEffect(ParticleEffect* effect);

        // Gets
        Texture2DGUI* GetTexture() const;

    private:
        friend class ParticleRenderTask;

        ParticleEffect* m_pParticleEffect = nullptr;
    };
}
#endif
