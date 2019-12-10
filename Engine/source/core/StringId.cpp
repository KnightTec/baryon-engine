#include "StringId.h"

namespace Baryon
{
StringId::StringId(const char* str) : hash(computeHash(str, strlen(str)))
{
	if (stringIdMap.find(hash) == stringIdMap.end())
	{
		stringIdMap[hash] = _strdup(str);
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
