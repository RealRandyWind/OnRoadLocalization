#include "Allocator.hpp"
#include <stdlib.h>

Allocator* Allocator::a_oDefault = 0;

Allocator::Allocator()
{

}

Allocator::~Allocator()
{

}

void* Allocator::Allocate(size_t nSize)
{
	if (!nSize) { return NULL; }
	return malloc(nSize);
}

void* Allocator::ReAllocate(void* oTarget, size_t nSize)
{
	return realloc(oTarget, nSize);
}

void Allocator::DeAllocate(void* oTarget)
{
	delete[] oTarget;
}

Allocator* Allocator::GetDefault()
{
	if (!a_oDefault) { a_oDefault = new Allocator(); }
	return a_oDefault;
}