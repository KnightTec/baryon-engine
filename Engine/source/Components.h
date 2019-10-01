#pragma once
#include "DirectXMath.h"
#include "Mesh.h"

#include <unordered_map>

namespace Baryon
{
typedef const void* TypeId;
template <typename T>
TypeId typeId(T obj = T());

typedef unsigned long long TypeFlag;
typedef uint64_t EntityId;

struct TypeInfo
{
	const char* name;
	TypeFlag flag;
	size_t sizeInBytes;
	size_t alignment;
};

class ComponentRegistry
{
public:
	template <typename T>
	static void registerComponentType(const char* typeName, size_t componentsPerChunk = 4096);
	template <typename T>
	static const TypeInfo& getTypeInfo();
	static const TypeInfo& getTypeInfo(TypeFlag flag);
	static const TypeInfo& getTypeInfo(TypeId typeId);
	static void initialize();
private:
	static std::unordered_map<TypeId, TypeInfo> idMap;
	static std::unordered_map<TypeFlag, TypeInfo> flagMap;
};


template <typename T>
TypeId typeId(T obj)
{
	static const char typeId = 0;
	return &typeId;
}

template <typename T>
void ComponentRegistry::registerComponentType(const char* typeName, size_t componentsPerChunk)
{
	TypeId id = typeId<T>();
	if (idMap.find(id) == idMap.end())
	{
		auto& typeInfo = idMap[typeId<T>()];
		typeInfo.name = typeName;
		typeInfo.flag = 1 << (idMap.size() - 1);
		typeInfo.sizeInBytes = sizeof T;
		typeInfo.alignment = alignof(T);

		flagMap[typeInfo.flag] = typeInfo;
	}
}
template <typename T>
const TypeInfo& ComponentRegistry::getTypeInfo()
{
	assert(idMap.find(typeId<T>()) != idMap.end());
	return idMap[typeId<T>()];
}
inline const TypeInfo& ComponentRegistry::getTypeInfo(TypeFlag flag)
{
	assert(flagMap.find(flag) != flagMap.end());
	return flagMap[flag];
}


//TODO: use static variables to autoregister types outside functions

#define REGISTER_COMPONENT_TYPE_1_ARG(type)							ComponentRegistry::registerComponentType<type>(#type)
#define REGISTER_COMPONENT_TYPE_2_ARGS(type, componentsPerChunk)	ComponentRegistry::registerComponentType<type>(#type, componentsPerChunk)
#define GET_3RD_ARG(arg1, arg2, arg3, ...) arg3
#define REGISTER_COMPONENT_TYPE(...) GET_3RD_ARG(__VA_ARGS__, REGISTER_COMPONENT_TYPE_2_ARGS, REGISTER_COMPONENT_TYPE_1_ARG)(__VA_ARGS__)


struct Transform
{
	DirectX::XMFLOAT4 rotationQuaternion;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 size;
};

struct StaticMesh
{
	DirectX::XMFLOAT4X3 worldMatrix;
	//TODO: replace with path
	Mesh* mesh;
};
}
