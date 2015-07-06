#ifndef MSDLMANAGER_HPP
#define MSDLMANAGER_HPP
#define GL_GLEXT_PROTOTYPES
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "SDL/SDL.h"
#undef main
#include "IResource.hpp"
#include "MLogManager.hpp"

class MSDLManager;
class MSDLManager
{
private:
	MSDLManager();
	static MSDLManager* a_oInstance;
	bool a_bIsSetUp;
	bool a_bIsShutDown;

	MLogManager* a_mLogManager;

	bool a_bIsFullScreen;

	unsigned int a_iScreenHeight;
	unsigned int a_iScreenWidth;

	Uint32 a_sdlWindowFlags;
	SDL_Window* a_sdlWindow;
	SDL_Renderer* a_sdlRenderer;
	SDL_Texture* a_sdlTexture;
	int a_sdlLastTicks;
	int a_sdlDTicks;
	SDL_Event a_sdlEvent;

	int a_sdlError;
public:
	~MSDLManager();
	static MSDLManager* GetInstance();

	void SetUp();
	void ShutDown();

	bool IsFullScreen(bool bIsFullScreen);
	bool IsFullScreen();

	bool IsExit();

	unsigned int CheckEvent();

	void SetSize(unsigned int iWidth, unsigned int iHeight);
	void Display(uint8_t* oImage);
	void Display(IResource* oResource); /* TEMP maybe removed */

	int GetDelta();
};
#endif
