#pragma once
#include <fstream>

namespace Baryon
{
namespace IOUtil
{
	template <typename T>
	void writeValue(std::ofstream& stream, T value)
	{
		stream.write(reinterpret_cast<char*>(&value), sizeof T);
	}
	template <typename T>
	void readValue(std::ifstream& stream, T& value)
	{
		stream.read(reinterpret_cast<char*>(&value), sizeof T);
	}
}
}
