#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

class ParticleEffect;
class DescriptorHeap;

namespace component
{
	class ParticleEmitterComponent;
}

class ParticleSystem
{
public:
	static ParticleSystem& GetInstance();
	~ParticleSystem();

	void Update(double dt);

	void SetParticleEffect(ParticleEffect* effect);

	void OnResetScene(); // Kanske, Jocke?

private:
	ParticleSystem();

	// Temp code todo: vector of effectComponents. component::init() pushes in.
	ParticleEffect* effect = nullptr;

};

#endif
