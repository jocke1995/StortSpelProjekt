#ifndef MODELCOMPONENT_H
#define MODELCOMPONENT_H

#include <map>
#include "Component.h"
#include "structs.h"
#include "Core.h"
class Mesh;
class Model;
class Material;

namespace component
{
    class ModelComponent : public Component
    {
    public:
        ModelComponent(Entity* parent);
        virtual ~ModelComponent();

        void Update(double dt);
        void InitScene();

        // Sets
        void SetModel(Model* model);
        void SetDrawFlag(unsigned int drawFlag);

        // Gets
        Mesh* GetMeshAt(unsigned int index) const;
        Material* GetMaterialAt(unsigned int index) const;
        const SlotInfo* GetSlotInfoAt(unsigned int index) const;
        unsigned int GetDrawFlag() const;
        unsigned int GetNrOfMeshes() const;
        const std::wstring& GetModelPath() const;
        bool IsPickedThisFrame() const;

    private:
        // The boundingBox will update the "m_IsPickedThisFrame"
        friend class BoundingBoxComponent;
        friend class Engine;
        friend class Renderer;
        bool m_IsPickedThisFrame = false;

        Model* m_Model = nullptr;
        unsigned int m_DrawFlag = 0;
    };
}
#endif
