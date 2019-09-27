#pragma once
#include <memory>
#include <cassert>

namespace Baryon
{
template <typename T>
class Array
{
public:
	Array(size_t capacity);
	T& operator[](int index) const;
private:
	size_t capacity;
	T* data;
};

template <typename T>
Array<T>::Array(size_t capacity) : capacity{ capacity }, data{ malloc(capacity * sizeof(T)) }
{
}
template <typename T>
T& Array<T>::operator[](int index) const
{
	assert(index >= 0 && index < capacity);
	return data[index];
}
}
