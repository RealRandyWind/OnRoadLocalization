#ifndef MRESOURCEMANAGER_HPP
#define MRESOURCEMANAGER_HPP
#include "IResource.hpp"
#include "MLogManager.hpp"
#include "IResource.hpp"
#include <vector>
#include <string>

enum EResourceType{
	RESOURCE_MOVIE,

	RESOURCE_DEFAULT = RESOURCE_MOVIE
};


class MResourceManager;
class MResourceManager
{
private:
	MResourceManager();
	static MResourceManager* a_oInstance;
	bool a_bIsSetUp;
	bool a_bIsShutDown;

	MLogManager* a_mLogManager;

	std::vector<IResource*> a_lResources;

	void Fx_DestroyResource(IResource* oProgram);
public:
	~MResourceManager();
	static MResourceManager* GetInstance();

	void SetUp();
	void ShutDown();

	unsigned int LoadResourceFromFile(std::string sPath, unsigned int eType=RESOURCE_DEFAULT);

	IResource* Get(unsigned int iResource);
	void Destroy(unsigned int iResource = 0);
};
#endif