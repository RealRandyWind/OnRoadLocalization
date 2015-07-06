#ifndef MRenderManager_HPP
#define MRenderManager_HPP
#include "MLogManager.hpp"

class MRenderManager;
class MRenderManager
{
private:
	MRenderManager();
	static MRenderManager* a_oInstance;
	bool a_bIsSetUp;
	bool a_bIsShutDown;

	MLogManager* a_mLogManager;
public:
	~MRenderManager();
	static MRenderManager* GetInstance();

	void SetUp();
	void ShutDown();
};
#endif