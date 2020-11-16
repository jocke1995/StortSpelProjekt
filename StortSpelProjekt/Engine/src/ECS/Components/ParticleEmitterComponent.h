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
        ParticleEmitterComponent(Entity* parent, Texture2DGUI* texture, ParticleEffectSettings* settings, bool playOnInit = false);
        virtual ~ParticleEmitterComponent();

        void RenderUpdate(double dt);
        void OnInitScene();
        void OnUnInitScene();

        // Reset ParticleEffect
        void Play();
        void Stop();
        bool IsPlaying() const;

        // Gets
        const ParticleEffect* GetParticleEffect() const;

    private:
        friend class ParticleRenderTask;

        ParticleEffect m_ParticleEffect;
        bool m_PlayOnInit;
        
        bool m_IsPlaying = false;
    };
}
#endif
