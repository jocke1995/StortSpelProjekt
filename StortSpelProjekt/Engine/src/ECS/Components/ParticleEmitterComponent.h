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
        ParticleEmitterComponent(Entity* parent, std::vector<ParticleEffectSettings>* settings, bool playOnInit = false);
        ParticleEmitterComponent(Entity* parent, ParticleEffectSettings* settings, bool playOnInit = false);
        virtual ~ParticleEmitterComponent();

        void RenderUpdate(double dt);
        void OnInitScene();
        void OnUnInitScene();

        // Reset ParticleEffect
        void Play();
        void Play(double duration);
        void Stop();
        bool IsPlaying() const;

        // Gets
        const std::vector<ParticleEffect*>* GetParticleEffects() const;

    private:
        friend class ParticleRenderTask;

        std::vector<ParticleEffect*> m_ParticleEffects;
        bool m_PlayOnInit;
        double m_PlayDuration;
        
        bool m_IsPlaying = false;
    };
}
#endif
