#include "Components.h"

using namespace Baryon;

std::unordered_map<TypeId, TypeInfo> ComponentRegistry::idMap;
std::unordered_map<TypeFlag, TypeInfo> ComponentRegistry::flagMap;

void ComponentRegistry::initialize()
{
	REGISTER_COMPONENT_TYPE(Transform);
	REGISTER_COMPONENT_TYPE(StaticMesh);
}