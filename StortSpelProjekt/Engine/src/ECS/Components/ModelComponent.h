#ifndef MODELCOMPONENT_H
#define MODELCOMPONENT_H

#include "Component.h"
#include "structs.h"
class Mesh;
class Model;
class Texture;

namespace component
{
    class ModelComponent : public Component
    {
    public:
        ModelComponent(Entity* parent);
        virtual ~ModelComponent();

        void Update(double dt);

        // Sets
        void SetModel(Model* model);
        Mesh* GetMeshAt(unsigned int index);
        std::map<TEXTURE_TYPE, Texture*> GetTexturesAt(unsigned int index);
        SlotInfo GetSlotInfoAt(unsigned int index);
        
        void SetDrawFlag(unsigned int drawFlag);

        // Gets
        unsigned int GetDrawFlag() const;
        unsigned int GetNrOfMeshes() const;
        bool IsPickedThisFrame() const;

    private:
        // The boundingBox will update the "m_IsPickedThisFrame"
        friend class BoundingBoxComponent;
        bool m_IsPickedThisFrame = false;

        Model* m_Model = nullptr;
        unsigned int m_DrawFlag = 0;
    };
}
#endif
