#include "ComponentRegistry.h"
#include "components/Components.h"
#include "components/Transform.h"

using namespace Baryon;

std::unordered_map<TypeId, TypeInfo> ComponentRegistry::idMap;
std::unordered_map<TypeFlag, TypeInfo> ComponentRegistry::flagMap;


REGISTER_COMPONENT_TYPE(Transform)
REGISTER_COMPONENT_TYPE(MeshComponent)
REGISTER_COMPONENT_TYPE(WorldMatrixComponent)