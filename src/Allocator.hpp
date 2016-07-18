#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

class Allocator;
class Allocator
{
	static Allocator* a_oDefault;
public:
	Allocator();
	~Allocator();

	void* Allocate(size_t nSize);
	template<typename T>
	T* Allocate(size_t nSize) { return (T*)Allocate(sizeof(T) * nSize); };

	void* ReAllocate(void* oTarget, size_t nSize);
	template<typename T>
	T* ReAllocate(void* oTarget, size_t nSize) { return (T*)ReAllocate(oTarget, sizeof(T) * nSize); };

	void DeAllocate(void* oTarget);

	static Allocator* GetDefault();
};
#endif