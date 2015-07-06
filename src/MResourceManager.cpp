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
		a_mLogManager->Warning(0, "[MResourceManager.SetUp] already setup.");
		return;
	}
	a_bIsShutDown = false;

	// TODO MResourceManager SetUp

	a_bIsSetUp = true;
	a_mLogManager->Success(0, "[MResourceManager.SetUp] done.");
}

void MResourceManager::ShutDown()
{
	if(a_bIsShutDown) {
		a_mLogManager->Warning(0, "[MResourceManager.ShutDown] already shutdown.");
		return; 
	}
	a_bIsSetUp = false;

	// TODO MResourceManager ShutDown

	a_bIsShutDown = true;
	a_mLogManager->Success(0, "[MResourceManager.ShutDown] done.");
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
		a_mLogManager->Warning(0, "[MResourceManager.LoadResourceFromFile] no resource is counstructed.");
		return -1;
	}

	unsigned int iResource = (unsigned int)a_lResources.size();
	a_lResources.push_back(oResource);

	return iResource;
}

IResource* MResourceManager::Get(unsigned int iResource)
{
	IResource* oResource;

	if (iResource < 0 || a_lResources.size() <= iResource) {
		a_mLogManager->Warning(0, "[MResourceManager.Get] resource %d not in scope.", iResource);
		return NULL;
	}
	
	oResource = a_lResources[iResource];
	
	if (oResource == NULL)
	{
		a_mLogManager->Warning(0, "[MResourceManager.Get] resource is destroyed.");
	}

	return oResource;
}

void MResourceManager::Destroy(unsigned int iResource)
{
	if (iResource < 0 || a_lResources.size() <= iResource)
	{
		a_mLogManager->Warning(0, "[MResourceManager.Destroy] program %d not in scope.", iResource);
		return;
	}
	Fx_DestroyResource(a_lResources[iResource]);
	a_lResources[iResource] = NULL;
}