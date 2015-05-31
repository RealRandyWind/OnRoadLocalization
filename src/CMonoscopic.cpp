#include "CMonoscopic.hpp"
#include "MLogManager.hpp"
#include "MResourceManager.hpp"
#include "MOpenCLManager.hpp"

CMonoscopic::CMonoscopic()
{

}

CMonoscopic::~CMonoscopic()
{

}

void CMonoscopic::Run()
{
	MLogManager* mLogManager = MLogManager::GetInstance();
	MResourceManager* mResourceManager = MResourceManager::GetInstance();
	MOpenCLManager* mOpenCLManager = MOpenCLManager::GetInstance();
	
	unsigned int iMovie = mResourceManager->LoadResourceFromFile("D:/Repositories/onroadlocalization/resources/big_buck_bunny_180p.mp4");
	IResource* oMovie = mResourceManager->Get(iMovie);
}