#pragma once
#include "Transform.h"

namespace Baryon
{
class Entity
{
public:
	virtual void update() = 0;
protected:
	Transform transform;
private:
	uint64_t id;
};
}
