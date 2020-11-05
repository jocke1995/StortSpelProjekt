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

	void OnResetScene(); // Kanske, Jocke?

private:
	ParticleSystem();

	ParticleEffect* effect = nullptr;


	void uploadParticleEffectsToGPU();
};

#endif
