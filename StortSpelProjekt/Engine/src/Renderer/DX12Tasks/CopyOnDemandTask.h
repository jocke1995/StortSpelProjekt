#ifndef COPYONDEMANDTASK_H
#define COPYONDEMANDTASK_H

#include "CopyTask.h"
class Texture;
class TextData;
class Text;

class CopyOnDemandTask : public CopyTask
{
public:
	CopyOnDemandTask(ID3D12Device5* device, COMMAND_INTERFACE_TYPE interfaceType, unsigned int FLAG_THREAD);
	virtual ~CopyOnDemandTask();

	void SubmitTexture(Texture* texture);

	// Todo: Remove this funciton
	// Bad coding, have to have currently for text to work.
	void UnSubmitText(Text* text);

	// Removal
	void Clear();

	void Execute();

private:
	std::vector<Texture*> m_Textures;
	void copyTexture(ID3D12GraphicsCommandList5* commandList, Texture* texture);
};

#endif
