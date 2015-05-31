#ifndef IRESOURCE_HPP
#define IRESOURCE_HPP

class IResource;
class IResource
{
public:
	virtual ~IResource() {};
	virtual void* GetData() = 0;
	virtual void* GetData(void* pAt, unsigned int iCount=1) = 0;
	virtual void* GetRangeData(void* pFrom, void* pTo, unsigned int iCount=1) = 0;
	virtual void* GetDataDescriptor() = 0; /* IDataDescriptor* */
};
#endif