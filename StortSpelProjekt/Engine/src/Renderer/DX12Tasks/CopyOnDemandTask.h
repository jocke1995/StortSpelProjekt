#ifndef COPYONDEMANDTASK_H
#define COPYONDEMANDTASK_H

#include "CopyTask.h"
class Texture;

class CopyOnDemandTask : public CopyTask
{
public:
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
