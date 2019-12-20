#pragma once
#include "Entity.h"

namespace Baryon
{
class IScript
{
public:
	virtual ~IScript() = default;
	virtual void update() = 0;
};

template <typename T>
class EntityScript : public PooledType<T>, public IScript
{
public:
	EntityScript(Entity entity) : entity(entity)
	{
	}
	virtual ~EntityScript() = default;
	void update() override = 0;
protected:
	Entity entity;
};

struct ScriptComponent
{
	IScript* script;
};

}
