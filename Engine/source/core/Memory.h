#pragma once
#include <cstdint>
#include <cstdlib>

namespace Baryon
{
class StackAllocator
{
public:
	StackAllocator(size_t stackSize);
	~StackAllocator();

	StackAllocator(const StackAllocator&) = delete;
	StackAllocator(const StackAllocator&&) = delete;
	StackAllocator& operator=(const StackAllocator&) = delete;
	StackAllocator& operator=(const StackAllocator&&) = delete;

	void* allocate(size_t size);
	void clearStack();
private:
	const size_t stackSize;
	size_t allocatedSize;
	uint8_t* stackBase;
};

template <typename T, size_t BLOCK_SIZE = 256>
class PoolAllocator
{
	//TODO: add multiple pools
public:
	PoolAllocator(size_t poolSize = BLOCK_SIZE);
	~PoolAllocator();
	T* allocate();
	void free(T* ptr);
private:
	union Element
	{
		T data;
		Element* nextFree;
	};

	Element* pool;
	Element* firstFree;
};


inline StackAllocator::StackAllocator(size_t stackSize)
	: stackSize(stackSize), allocatedSize(0), stackBase(static_cast<uint8_t*>(malloc(stackSize)))
{
}
inline StackAllocator::~StackAllocator()
{
	free(stackBase);
}
inline void* StackAllocator::allocate(size_t size)
{
	size_t newAllocatedSize = allocatedSize + size;
	if (newAllocatedSize > stackSize)
	{
		return nullptr;
	}
	allocatedSize = newAllocatedSize;
	return stackBase + allocatedSize;
}
inline void StackAllocator::clearStack()
{
	allocatedSize = 0;
}

template <typename T, size_t BLOCK_SIZE>
PoolAllocator<T, BLOCK_SIZE>::PoolAllocator(size_t poolSize)
	: pool(static_cast<Element*>(malloc(poolSize * sizeof Element)))
{
	firstFree = &pool[0];
	for (size_t i = 0; i < poolSize - 1; i++)
	{
		pool[i].nextFree = &pool[i + 1];
	}
	pool[poolSize - 1].nextFree = nullptr;
}
template <typename T, size_t BLOCK_SIZE>
PoolAllocator<T, BLOCK_SIZE>::~PoolAllocator()
{
	::free(pool);
}
template <typename T, size_t BLOCK_SIZE>
T* PoolAllocator<T, BLOCK_SIZE>::allocate()
{
	assert(firstFree != nullptr);
	Element* newT = firstFree;
	firstFree = newT->nextFree;
	return &newT->data;
}
template <typename T, size_t BLOCK_SIZE>
void PoolAllocator<T, BLOCK_SIZE>::free(T* ptr)
{
	auto freed = reinterpret_cast<Element*>(ptr);
	freed->nextFree = firstFree;
	firstFree = freed;
}
}
