#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

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

	void OnResetScene(); // Kanske

private:
	ParticleSystem();
};

#endif
