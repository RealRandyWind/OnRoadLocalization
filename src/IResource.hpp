#ifndef IRESOURCE_HPP
#define IRESOURCE_HPP
#define NULL 0
#include "IDataDescriptor.hpp"
#include <stdint.h>

class IResource;
class IResource
{
public:
	virtual ~IResource() {};
	virtual uint8_t* GetData(uint8_t* oDestination=NULL) = 0;
	virtual void Display(void** oDestination, int iPitch) = 0;
	virtual IDataDescriptor* GetDataDescriptor() = 0;
};
#endif