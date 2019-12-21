#pragma once
#include "System.h"
#include "components/Transform.h"
#include "components/Components.h"


namespace Baryon
{
class WorldMatrixSystem : public System<Transform, WorldMatrixComponent>
{
	using super = System<Transform, WorldMatrixComponent>;
public:
	WorldMatrixSystem(EntityManager* entityManager);
private:
	void update(Transform& transform, WorldMatrixComponent& wmc) override;
};
}
