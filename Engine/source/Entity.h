#pragma once
#include <cstdint>

namespace Baryon
{
class Entity
{
public:
	//Entity(const Prototype* prototype);

	Entity() = default;
	//virtual void update() = 0;
	uint64_t getId() const;

private:
	uint64_t id;
};

inline uint64_t Entity::getId() const
{
	return id;
}

}
