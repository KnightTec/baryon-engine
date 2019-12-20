#include "ComponentRegistry.h"
#include "components/Components.h"
#include "components/Transform.h"
#include "components/CameraComponent.h"
#include "components/ScriptComponent.h"

using namespace Baryon;

std::unordered_map<TypeId, TypeInfo> ComponentRegistry::idMap;
std::unordered_map<TypeFlag, TypeInfo> ComponentRegistry::flagMap;


REGISTER_COMPONENT_TYPE(Transform, 4096)
REGISTER_COMPONENT_TYPE(MeshComponent, 4096)
REGISTER_COMPONENT_TYPE(WorldMatrixComponent, 4096)
REGISTER_COMPONENT_TYPE(CameraComponent, 16)
REGISTER_COMPONENT_TYPE(ScriptComponent, 64)