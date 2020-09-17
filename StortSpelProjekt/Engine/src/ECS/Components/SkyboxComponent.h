#ifndef SKYBOXCOMPONENT_H
#define SKYBOXCOMPONENT_H

#include "Component.h"
#include "structs.h"

class Mesh;
class Model;
class Texture;
class Transform;

namespace component
{
    class SkyboxComponent : public Component
    {
    public:
        SkyboxComponent(Entity* parent);
        virtual ~SkyboxComponent();

        void Update(double dt);

        // Sets
        void SetModel(Model* model);

        // Gets
        Transform* GetTransform() const;
        Mesh* GetMeshAt(unsigned int index) const;
        std::map<TEXTURE2D_TYPE, Texture*>* GetTexturesAt(unsigned int index) const;
        SlotInfo* GetSlotInfoAt(unsigned int index) const;
        unsigned int GetNrOfMeshes() const;

    private:
        Model* m_Model = nullptr;
        Transform* m_pTransform = nullptr;
    };
}
#endif
