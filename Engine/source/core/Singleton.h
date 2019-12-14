#pragma once

namespace Baryon
{
template <typename T>
class Singleton
{
public:
	Singleton(const Singleton&) = delete;
	Singleton(const Singleton&&) = delete;
	Singleton operator=(const Singleton&) = delete;
	Singleton operator=(const Singleton&&) = delete;

	static T& get()
	{
		static T instance;
		return instance;
	}
protected:
	Singleton() = default;
	~Singleton() = default;
};
}
