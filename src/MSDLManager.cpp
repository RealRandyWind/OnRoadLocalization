#include "MSDLManager.hpp"

MSDLManager* MSDLManager::a_oInstance = 0;

MSDLManager::MSDLManager()
{
	a_bIsShutDown = false;
	a_bIsSetUp = false;

	a_mLogManager = MLogManager::GetInstance();

	a_bIsFullScreen = false;

	a_sdlWindowFlags = SDL_WINDOW_SHOWN;
	a_iScreenHeight = 720;
	a_iScreenWidth = 1280;

	a_sdlWindow = NULL;
	a_sdlRenderer = NULL;
	a_sdlTexture = NULL;
	a_sdlLastTicks = SDL_GetTicks();
	a_sdlDTicks = 0;

	a_sdlError = 0;
}

MSDLManager::~MSDLManager()
{

}

MSDLManager* MSDLManager::GetInstance()
{
	if(a_oInstance) { return a_oInstance; }
	a_oInstance = new MSDLManager();
	return a_oInstance;
}

void MSDLManager::SetUp()
{
	if(a_bIsSetUp) {
		a_mLogManager->Warning(0, "[MSDLManager.SetUp] already setup.");
		return; 
	}
	a_bIsShutDown = false;

	a_sdlError = SDL_Init(SDL_INIT_EVERYTHING);
	if (a_sdlError) {
		a_mLogManager->Error(0, "[MSDLManager.SetUp] unable to initialise SDL, %s.",SDL_GetError());
		return;
	}

	a_sdlWindow = SDL_CreateWindow("TODO title", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, a_iScreenWidth, a_iScreenHeight, a_sdlWindowFlags);
	if (!a_sdlWindow) {
		a_mLogManager->Error(0, "[MSDLManager.SetUp] unable to create window, %s.", SDL_GetError());
		return;
	}

	a_sdlRenderer = SDL_CreateRenderer(a_sdlWindow, -1, SDL_RENDERER_ACCELERATED);
	if (!a_sdlRenderer) {
		a_mLogManager->Error(0, "[MSDLManager.SetUp] unable to create renderer, %s.", SDL_GetError());
		return;
	}

	SetSize(a_iScreenWidth,a_iScreenHeight);

	a_bIsSetUp = true;
	a_mLogManager->Success(0, "[MSDLManager.SetUp] done.");
}

void MSDLManager::ShutDown()
{
	if(a_bIsShutDown) {
		a_mLogManager->Warning(0, "[MSDLManager.ShutDown] already shutdown.");
		return; 
	}
	a_bIsSetUp = false;
	if (a_sdlTexture) {
		SDL_DestroyTexture(a_sdlTexture);
		a_sdlTexture = NULL;
	}

	if (a_sdlRenderer) {
		SDL_DestroyRenderer(a_sdlRenderer);
		a_sdlRenderer = NULL;
	}

	if (a_sdlWindow) { 
		SDL_DestroyWindow(a_sdlWindow);
		a_sdlWindow = NULL;
	}

	SDL_Quit();

	a_bIsShutDown = true;
	a_mLogManager->Success(0, "[MSDLManager.ShutDown] done.");
}

bool MSDLManager::IsFullScreen(bool bIsFullScreen)
{
	a_bIsFullScreen = bIsFullScreen;
	/*if(bIsFullScreen) { a_sdlVideoModeFlags |= SDL_FULLSCREEN; }
	else { a_sdlVideoModeFlags = a_sdlVideoModeFlags & (~SDL_FULLSCREEN); }*/
	return IsFullScreen();
}

bool MSDLManager::IsFullScreen()
{
	return a_bIsFullScreen;
}

void MSDLManager::SetSize(unsigned int iWidth, unsigned int iHeight)
{
	a_iScreenWidth = iWidth;
	a_iScreenHeight = iHeight;

	if (a_sdlTexture) {
		SDL_DestroyTexture(a_sdlTexture);
		a_sdlTexture = NULL;
	}
	
	/* SDL_PIXELFORMAT_RGBA8888  SDL_PIXELFORMAT_RGBA4444 SDL_PIXELFORMAT_RGB444*/
	a_sdlTexture = SDL_CreateTexture(a_sdlRenderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, a_iScreenWidth, a_iScreenHeight);
	
	if (!a_sdlTexture) {
		a_mLogManager->Error(0, "[MSDLManager.SetSize] unable to create streaming texture, %s.", SDL_GetError());
		return;
	}

	SDL_SetWindowSize(a_sdlWindow, iWidth, iHeight);
}

unsigned int MSDLManager::CheckEvent()
{
	return SDL_PollEvent(&a_sdlEvent);
}

bool MSDLManager::IsExit()
{
	return a_sdlEvent.type == SDL_QUIT;
}

/* refactor */
void MSDLManager::Display(uint8_t* oImage)
{
	if(!oImage) {
		a_mLogManager->Warning(0, "[MSDLManager.Display] trying to dispaly NULL.");
		return;
	}

	int iDestinationPitch = -1;
	unsigned int iSize = a_iScreenHeight * a_iScreenWidth;

	void* oDestinationImage = NULL;

	if (SDL_LockTexture(a_sdlTexture, NULL, &oDestinationImage, &iDestinationPitch) < 0) {
		a_mLogManager->Error(0, "[MSDLManager.Display] unable to lock texture, %s.", SDL_GetError());
		return;
	}

	memcpy(oDestinationImage, oImage, a_iScreenHeight * a_iScreenWidth * 4);

	SDL_UnlockTexture(a_sdlTexture);

	SDL_RenderClear(a_sdlRenderer);
	SDL_RenderCopy(a_sdlRenderer, a_sdlTexture, NULL, NULL);
	SDL_RenderPresent(a_sdlRenderer);
}

void MSDLManager::Display(IResource* oResource)
{
	if (!oResource) {
		a_mLogManager->Warning(0, "[MSDLManager.Display] trying to dispaly NULL as resources.");
		return;
	}

	int iDestinationPitch = -1;
	void* oDestinationImage = NULL;

	if (SDL_LockTexture(a_sdlTexture, NULL, &oDestinationImage, &iDestinationPitch) < 0) {
		a_mLogManager->Error(0, "[MSDLManager.Display] unable to lock texture, %s.", SDL_GetError());
		return;
	}

	oResource->Display(&oDestinationImage, iDestinationPitch);

	SDL_UnlockTexture(a_sdlTexture);

	SDL_RenderClear(a_sdlRenderer);
	SDL_RenderCopy(a_sdlRenderer, a_sdlTexture, NULL, NULL);
	SDL_RenderPresent(a_sdlRenderer);
}

int MSDLManager::GetDelta()
{
	return a_sdlDTicks;
}