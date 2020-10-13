#ifndef CALCULATEHEIGHTMAPNORMALSTASK_H
#define CALCULATEHEIGHTMAPNORMALSTASK_H

#include "MultiThreadedTask.h"
struct Vertex;
class CalculateHeightmapNormalsTask : public MultiThreadedTask
{
public:
	CalculateHeightmapNormalsTask(unsigned int id, unsigned int nrOfThreads, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, unsigned int width, unsigned int height);
	void Execute();
private:
	unsigned int m_Id;
	unsigned int m_NrOfThreads;
	std::vector<Vertex>& m_pVertices;
	std::vector<unsigned int>& m_pIndices;
	unsigned int m_Width;
	unsigned int m_Height;
};

#endif