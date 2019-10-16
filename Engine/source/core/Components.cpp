#include "Components.h"

using namespace Baryon;

std::unordered_map<TypeId, TypeInfo> ComponentRegistry::idMap;
std::unordered_map<TypeFlag, TypeInfo> ComponentRegistry::flagMap;


REGISTER_COMPONENT_TYPE_X(Transform)
REGISTER_COMPONENT_TYPE_X(StaticMesh)


void ComponentRegistry::initialize()
{
	//REGISTER_COMPONENT_TYPE(Transform);
	//REGISTER_COMPONENT_TYPE(StaticMesh);
}