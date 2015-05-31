#include "MResourceManager.hpp"
#include "RMovie.hpp"

#define MRESOURCE_READLINE_SIZE 256

MResourceManager* MResourceManager::a_oInstance = 0;

MResourceManager::MResourceManager()
{
	a_bIsShutDown = false;
	a_bIsSetUp = false;

	a_mLogManager = MLogManager::GetInstance();
}

MResourceManager::~MResourceManager()
{

}

MResourceManager* MResourceManager::GetInstance()
{
	if(a_oInstance) { return a_oInstance; }
	a_oInstance = new MResourceManager();
	return a_oInstance;
}

void MResourceManager::SetUp()
{
	if(a_bIsSetUp) {
		return; 
	}
	a_bIsShutDown = false;

	// TODO MResourceManager SetUp

	a_bIsSetUp = true;
}

void MResourceManager::ShutDown()
{
	if(a_bIsShutDown) {
		return; 
	}
	a_bIsSetUp = false;

	// TODO MResourceManager ShutDown

	a_bIsShutDown = true;
}

void MResourceManager::Fx_DestroyResource(IResource* oResource)
{
	delete oResource;
}

unsigned int MResourceManager::LoadResourceFromFile(std::string sPath, unsigned int eType)
{
	IResource* oResource;

	switch (eType)
	{
		case RESOURCE_MOVIE: oResource = new RMovie(sPath);  break;
		default: oResource = NULL; break;
	}

	if (!oResource) {
		a_mLogManager->Log("Call to 'LoadResourceFromFile' MResourceManager no resource is counstructed", 0, LOG_WARNING);
		return -1;
	}

	unsigned int iResource = a_lResources.size();
	a_lResources.push_back(oResource);

	return iResource;
}

IResource* MResourceManager::Get(unsigned int iResource)
{
	IResource* oResource;

	if (iResource < 0 || a_lResources.size() <= iResource) {
		a_mLogManager->LogF(LOG_WARNING, 0, "MResourceManager resource %d not in scope", iResource);
		return NULL;
	}
	
	oResource = a_lResources[iResource];
	
	if (oResource == NULL)
	{
		a_mLogManager->Log("Call to 'Get' of MResourceManager, but resource is destroyed", 0, LOG_WARNING);
	}

	return oResource;
}

void MResourceManager::Destroy(unsigned int iResource)
{
	if (iResource < 0 || a_lResources.size() <= iResource)
	{
		a_mLogManager->LogF(LOG_WARNING, 0, "MOpenCLManager program %d not in scope", iResource);
		return;
	}
	Fx_DestroyResource(a_lResources[iResource]);
	a_lResources[iResource] = NULL;
}