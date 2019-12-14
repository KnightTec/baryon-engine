#pragma once
#include <cstdint>
#include <unordered_map>

namespace Baryon
{
/*
 * Hashed strings
 */
class StringId
{
	friend class StringIdSentry;
public:
	StringId(const char* str);
	const char* c_str() const;
	uint64_t getHash() const;
private:
	constexpr StringId(uint64_t hash);

	uint64_t hash;

	static constexpr uint64_t computeHash(const char* str, size_t length);
	static std::unordered_map<uint64_t, const char*> stringIdMap;

	friend constexpr uint64_t operator"" _id(const char* str, size_t length);
	friend constexpr bool operator==(const StringId& sid1, const StringId& sid2);
	friend constexpr bool operator==(const StringId& sid, uint64_t hash);
	friend constexpr bool operator==(uint64_t hash, const StringId& sid);
	friend constexpr bool operator!=(const StringId& sid1, const StringId& sid2);
	friend constexpr bool operator!=(const StringId& sid, uint64_t hash);
	friend constexpr bool operator!=(uint64_t hash, const StringId& sid);
};


inline const char* StringId::c_str() const
{
	return stringIdMap[hash];
}
inline uint64_t StringId::getHash() const
{
	return hash;
}
constexpr StringId::StringId(uint64_t hash) : hash(hash)
{
}
constexpr uint64_t StringId::computeHash(const char* str, size_t length)
{
	// FNV-1a hash
	uint64_t hash = 14695981039346656037ULL;
	for (size_t i = 0; i < length; i++)
	{
		hash ^= static_cast<uint64_t>(str[i]);
		hash *= 1099511628211ULL;
	}
	return hash;
}
constexpr uint64_t operator"" _id(const char* str, size_t length)
{
	return StringId::computeHash(str, length);
}
constexpr bool operator==(const StringId& sid1, const StringId& sid2)
{
	return sid1.hash == sid2.hash;
}
constexpr bool operator==(const StringId& sid, uint64_t hash)
{
	return sid.hash == hash;
}
constexpr bool operator==(uint64_t hash, const StringId& sid)
{
	return hash == sid.hash;
}
constexpr bool operator!=(const StringId& sid1, const StringId& sid2)
{
	return sid1.hash != sid2.hash;
}
constexpr bool operator!=(const StringId& sid, uint64_t hash)
{
	return sid.hash != hash;
}
constexpr bool operator!=(uint64_t hash, const StringId& sid)
{
	return hash != sid.hash;
}
}
