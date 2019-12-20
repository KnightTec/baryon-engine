#pragma once
#include <cstdint>
#include "EntityManager.h"

namespace Baryon
{
class Entity
{
public:
	Entity();
	Entity(uint64_t entityId, EntityManager* entityManager);

	uint64_t getId() const;

	template <typename T>
	T* getComponent();
	template <typename T>
	void addComponent();
private:
	uint64_t id;
	EntityManager* entityManager;
};


inline Entity::Entity() :id(0), entityManager(nullptr)
{
}
inline Entity::Entity(uint64_t entityId, EntityManager* entityManager)
	: id(entityId), entityManager(entityManager)
{
	
}
inline uint64_t Entity::getId() const
{
	return id;
}
template <typename T>
T* Entity::getComponent()
{
	return entityManager->getComponent<T>(id);
}
template <typename T>
void Entity::addComponent()
{
	entityManager->addComponents<T>(id);
}



}
