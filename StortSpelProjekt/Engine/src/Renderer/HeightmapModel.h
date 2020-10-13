#ifndef HEIGHTMAPMODEL_H
#define HEIGHTMAPMODEL_H
#include "Model.h"

class HeightmapModel : public Model
{
public:
    HeightmapModel(const std::wstring* path,
        SkeletonNode* rootNode, 
        std::map<unsigned int, VertexWeight>* perVertexBoneData,
        std::vector<Mesh*>* meshes,
        std::vector<Animation*>* animations,
        std::vector<Material*>* materials, float* heights);
    virtual ~HeightmapModel();
    const float* GetHeights();
private:
    float* m_pHeights;
};

#endif