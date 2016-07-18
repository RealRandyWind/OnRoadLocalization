#include "CNeuralNetwork.hpp"
#include "MLogManager.hpp"
#include "MMemoryManager.hpp"
#include "MResourceManager.hpp"
#include "MSDLManager.hpp"
#include "DDMovie.hpp"

/* computation monoscopic default settings */
#define CNEURALNETWORK_FRAME_SPAN 11
#define CNEURALNETWORK_FEATURE_COUNT 4
#define CNEURALNETWORK_UNIT_SIZE 4

/* computation monoscopic feature vector indices */
#define CMFV_CL0 0
#define CMFV_CL1 1
#define CMFV_CL2 2
#define CMFV_LAST 2

CNeuralNetwork::CNeuralNetwork()
{

}

CNeuralNetwork::~CNeuralNetwork()
{

}

void CNeuralNetwork::Run()
{
	/* initialize managers */
	MLogManager* mLogManager = MLogManager::GetInstance();
	MMemoryManager* mMemoryManager = MMemoryManager::GetInstance();
	MResourceManager* mResourceManager = MResourceManager::GetInstance();
	MSDLManager* mSDLManager = MSDLManager::GetInstance();

	mLogManager->Log(0, "[CNeuralNetwork.Run] Running");

	/* set helper variables */
	unsigned int iWriteIndex = CNEURALNETWORK_FRAME_SPAN - 1;

	/* initialization movie */
	unsigned int iMovie = mResourceManager->LoadResourceFromFile("D:/Repositories/onroadlocalization/resources/Scotland Scenic Drives 42.  Stranraer to Cairnryan (via A77)-JWys4j1BjC4.mp4");
	IResource* oMovie = mResourceManager->Get(iMovie);
	DDMovie* oDDMovie = (DDMovie*)oMovie->GetDataDescriptor();
	DDMovie* oDDSubMovie = new DDMovie(oDDMovie);
	oDDSubMovie->a_iMaxFrames = CNEURALNETWORK_FRAME_SPAN;

	mSDLManager->SetSize(oDDMovie->a_iWidth, oDDMovie->a_iHeight);

	/* initialization buffers */
	size_t iSizeFrame = oDDMovie->a_iHeight * oDDMovie->a_iWidth;
	unsigned int iBytesFrame = iSizeFrame * sizeof(glm::u8vec4);
	uint8_t* oDisplay = (uint8_t*)mMemoryManager->Allocate(iBytesFrame);
	mLogManager->Info(0, "[CNeuralNetwork.Run] movie dimesions and fps (%i,%i,%f).", oDDMovie->a_iHeight, oDDMovie->a_iWidth, oDDMovie->a_dFps);
	
	unsigned int iBytesFrameBuffer = CNEURALNETWORK_FRAME_SPAN * iBytesFrame;
	glm::u8vec4* oFrameBuffer = (glm::u8vec4*)mMemoryManager->Allocate(iBytesFrameBuffer);
	uint8_t* oFrame = (uint8_t*)oFrameBuffer;

	unsigned int iBytesFrameIndexMap = sizeof(unsigned int) * CNEURALNETWORK_FRAME_SPAN;
	unsigned int* liFrameIndexMap = (unsigned int*)mMemoryManager->Allocate(iBytesFrameIndexMap);

	unsigned int iBytesFreatureVector = sizeof(float) * CNEURALNETWORK_FEATURE_COUNT * iSizeFrame;
	float* ldFreatureVector = (float*)mMemoryManager->Allocate(iBytesFreatureVector);

	/* initialization neural network */
	

	/* run */
	while (oMovie->GetData(oFrame)) {
		if (!mSDLManager->CheckEvent() && mSDLManager->IsExit()) { break; }

		/* put cap to frames fed to buffer and determine index map */
		oDDSubMovie->a_iFrames = (oDDMovie->a_iFrames >= CNEURALNETWORK_FRAME_SPAN ? CNEURALNETWORK_FRAME_SPAN : oDDMovie->a_iFrames);
		oDDSubMovie->a_iFrame = iWriteIndex;
		liFrameIndexMap[iWriteIndex] = iWriteIndex * iSizeFrame;
		
		/* compute */
		

		/* visualize */
		mSDLManager->Display(oFrame);

		/* swap buffer */
		iWriteIndex = CNEURALNETWORK_FRAME_SPAN - (oDDMovie->a_iFrames % CNEURALNETWORK_FRAME_SPAN) - 1;
		oFrame = (uint8_t*)oFrameBuffer + iWriteIndex * iBytesFrame;
	}

	/* clean up */
	mMemoryManager->DeAllocate(oFrameBuffer);
	mMemoryManager->DeAllocate(liFrameIndexMap);
	mMemoryManager->DeAllocate(ldFreatureVector);
	mMemoryManager->DeAllocate(oDisplay);

	mLogManager->Log(0, "[CNeuralNetwork.Run] Stopped");
}