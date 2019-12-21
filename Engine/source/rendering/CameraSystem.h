#pragma once
#include "System.h"
#include "components/Transform.h"
#include "components/CameraComponent.h"

namespace Baryon
{
class CameraSystem : public System<Transform, CameraComponent>
{
	using super = System<Transform, CameraComponent>;
public:
	CameraSystem(EntityManager* entityManager);
private:
	void update(Transform& transform, CameraComponent& camera) override;
};
}
