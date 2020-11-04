#ifndef HEIGHTMAPMODEL_H
#define HEIGHTMAPMODEL_H
#include "Model.h"

class HeightmapModel : public Model
{
public:
    HeightmapModel(const std::wstring* path,
        std::vector<Mesh*>* meshes,
        std::vector<Material*>* materials, double* heights, double width, double length);
    virtual ~HeightmapModel();
    const double* GetHeights();
    const double GetWidth();
    const double GetLength();
private:
    double* m_pHeights;
    double m_Width;
    double m_Length;
};

#endif