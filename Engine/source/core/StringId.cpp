#include "StringId.h"
#include <cassert>

namespace Baryon
{
std::unordered_map<uint64_t, const char*> StringId::stringIdMap;

StringId::StringId(const char* str) : hash(computeHash(str, strlen(str)))
{
	auto it = stringIdMap.find(hash);
	if (it == stringIdMap.end())
	{
		stringIdMap[hash] = _strdup(str);
	}
	else
	{
		assert(strcmp(str, it->second) == 0);
	}
}

/*
 * Is responsile for freeing all dynamically allocated string memory at the end of the program
 */
class StringIdSentry
{
public:
	~StringIdSentry()
	{
		for (auto& pair : StringId::stringIdMap)
		{
			free(const_cast<char*>(pair.second));
		}
	}
};

static StringIdSentry sentry;
}
