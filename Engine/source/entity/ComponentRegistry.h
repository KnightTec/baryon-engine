#pragma once
#include "DirectXMath.h"
#include "Mesh.h"

#include <unordered_map>

namespace Baryon
{
typedef const void* TypeId;

typedef long long TypeFlag;
typedef uint64_t EntityId;

struct TypeInfo
{
	using Constructor = void(void*);

	TypeId typeId;
	const char* name;
	TypeFlag flag;
	size_t sizeInBytes;
	size_t alignment;
	int countPerChunk;
	Constructor* constructor;
};

class ComponentRegistry
{
public:
	static void initialize();
	template <typename T>
	static void registerComponentType(const char* typeName, size_t countPerChunk);
	template <typename T>
	static const TypeInfo& getTypeInfo();
	static const TypeInfo& getTypeInfo(TypeFlag flag);
	static const TypeInfo& getTypeInfo(TypeId typeId);
private:
	static std::unordered_map<TypeId, TypeInfo> idMap;
	static std::unordered_map<TypeFlag, TypeInfo> flagMap;
};

namespace Internal
{
extern int _typeIdCounter;
}

template <typename T>
TypeId typeId()
{
	static const int typeId = Internal::_typeIdCounter++;
	return &typeId;
}

//namespace Internal
//{
//extern int _typeCounter;
//
//template <typename T>
//TypeInfo& initTypeInfo(const char* typeName = nullptr, size_t countPerChunk = 0)
//{
//	static TypeInfo typeInfo = {
//		typeId<T>(),
//		typeName,
//		1ll << ++_typeCounter,
//		sizeof T,
//		alignof(T),
//		countPerChunk,
//		[](void* p) { new(p) T(); }
//	};
//	return typeInfo;
//}
//}
//
//template <typename T>
//const TypeInfo& typeInfo()
//{
//	return Internal::initTypeInfo<T>();
//}

template <typename T>
void ComponentRegistry::registerComponentType(const char* typeName, size_t countPerChunk)
{
	TypeId id = typeId<T>();
	if (idMap.find(id) == idMap.end())
	{
		auto& typeInfo = idMap[typeId<T>()];
		typeInfo.typeId = typeId<T>();
		typeInfo.name = typeName;
		typeInfo.flag = 1ll << (idMap.size() - 1);
		typeInfo.sizeInBytes = sizeof T;
		typeInfo.alignment = alignof(T);
		typeInfo.countPerChunk = countPerChunk;
		typeInfo.constructor = [](void* p)
		{
			new(p) T();
		};
		flagMap[typeInfo.flag] = typeInfo;
	}
}
template <typename T>
const TypeInfo& ComponentRegistry::getTypeInfo()
{
	return getTypeInfo(typeId<T>());
}
inline const TypeInfo& ComponentRegistry::getTypeInfo(TypeFlag flag)
{
	assert(flagMap.find(flag) != flagMap.end());
	return flagMap[flag];
}
inline const TypeInfo& ComponentRegistry::getTypeInfo(TypeId typeId)
{
	assert(idMap.find(typeId) != idMap.end());
	return idMap[typeId];
}
}
