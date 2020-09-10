#ifndef MESHCOMPONENT_H
#define MESHCOMPONENT_H

#include "Component.h"
class Mesh;
namespace component
{
    class MeshComponent : public Component
    {
    public:
        MeshComponent(Entity* parent);
        virtual ~MeshComponent();

        void Update(double dt);

        // Sets
        void SetMeshes(std::vector<Mesh*>* meshes);
        
        void SetDrawFlag(unsigned int drawFlag);

        // Gets
        Mesh* GetMesh(unsigned int index) const;
        unsigned int GetDrawFlag() const;
        unsigned int GetNrOfMeshes() const;
        bool IsPickedThisFrame() const;

    private:
        // The boundingBox will update the "m_IsPickedThisFrame"
        friend class BoundingBoxComponent;
        bool m_IsPickedThisFrame = false;

        std::vector<Mesh*> m_Meshes;
        unsigned int m_DrawFlag = 0;
    };
}
#endif
