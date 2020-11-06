#ifndef PARTICLEEMITTERCOMPONENT_H
#define PARTICLEEMITTERCOMPONENT_H

#include "Component.h"
#include "structs.h"

class Mesh;
class Transform;
class Texture2DGUI;
class ParticleEffect;

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
