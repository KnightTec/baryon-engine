#pragma once
#include "System.h"
#include "components/ScriptComponent.h"

namespace Baryon
{
class ScriptSystem : public System<ScriptComponent>
{
	using super = System<ScriptComponent>;
public:
	ScriptSystem(EntityManager* entityManager);
	void update(ScriptComponent& scriptComponent) override;
};

inline ScriptSystem::ScriptSystem(EntityManager* entityManager) : super(entityManager)
{
}
inline void ScriptSystem::update(ScriptComponent& scriptComponent)
{
	scriptComponent.script->update();
}
}
