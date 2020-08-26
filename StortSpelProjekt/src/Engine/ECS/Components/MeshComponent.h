#ifndef MESHCOMPONENT_H
#define MESHCOMPONENT_H

#include "../../Renderer/Mesh.h"
#include "Component.h"

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
        Mesh* GetMesh(unsigned int index = 0) const;
        unsigned int GetDrawFlag() const;
        unsigned int GetNrOfMeshes() const;
        bool IsPickedThisFrame() const;

    private:
        // The boundingBox will update the "isPickedThisFrame"
        friend class BoundingBoxComponent;
        bool isPickedThisFrame = false;

        std::vector<Mesh*> meshes;
        unsigned int drawFlag = 0;
    };
}
#endif