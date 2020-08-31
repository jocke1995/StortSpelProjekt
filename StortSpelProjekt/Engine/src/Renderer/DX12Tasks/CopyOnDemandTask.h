#ifndef COPYONDEMANDTASK_H
#define COPYONDEMANDTASK_H

#include "CopyTask.h"
class ID3D12Device5;
class Texture;

class CopyOnDemandTask : public CopyTask
{
	CopyOnDemandTask(ID3D12Device5* device);
	virtual ~CopyOnDemandTask();

	void SubmitTexture(Texture* texture);

	// Removal
	void Clear();

	void Execute();

private:
	std::vector<Texture*> m_Textures;
	void copyTexture(Texture* texture);
};

#endif
