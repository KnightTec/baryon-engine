#pragma once
#include "EntityManager.h"
#include <tuple>
#include <utility>

namespace Baryon
{
class ISystem
{
public:
	explicit ISystem(EntityManager* entityManager);
	virtual ~ISystem() = default;
	virtual void initialize()
	{
	}
	virtual void tick() = 0;
	virtual void terminate()
	{
	}
protected:
	EntityManager* entityManager;
};

inline ISystem::ISystem(EntityManager* entityManager)
	: entityManager(entityManager)
{
}


template <typename... HandledComponents>
class System : public ISystem
{
public:
	System(EntityManager* entityManager);
	virtual void tick() override;
protected:
	virtual void update(HandledComponents&...) = 0;
private:
	template <size_t...I>
	void callUpdateAtIndex(int index, std::index_sequence<I...>)
	{
		update(std::get<I>(componentArrays)[index]...);
	}

	std::vector<TypeId> handledTypeIds;
	TypeFlag componentFlags;
	Archetype* archetypePtrBuffer[64];

	std::tuple<HandledComponents*...> componentArrays;
};


template <typename... HandledComponents>
System<HandledComponents...>::System(EntityManager* entityManager)
	: ISystem(entityManager), componentFlags(0), archetypePtrBuffer{nullptr}
{
	using expander = int[];
	(void)expander{
		0, ((void)(componentFlags |= ComponentRegistry::getTypeInfo(typeId<HandledComponents>()).flag), 0)...
	};
}
template <typename ... HandledComponents>
void System<HandledComponents...>::tick()
{
	int archetypeCount = 64;
	entityManager->getArchetypesWithComponents(componentFlags, archetypePtrBuffer, archetypeCount);
	auto indexSeq = std::make_index_sequence<std::tuple_size<decltype(componentArrays)>{}>{};
	for (int i = 0; i < archetypeCount; i++)
	{
		componentArrays = std::make_tuple(archetypePtrBuffer[i]->getComponentArray<HandledComponents>()...);
		int entityCount = archetypePtrBuffer[i]->getEntityCount();
		for (int k = 0; k < entityCount; k++)
		{
			callUpdateAtIndex(k, indexSeq);
		}
	}
}
}
