#include "CStereoscopic.hpp"
#include "MLogManager.hpp"
#include "MMemoryManager.hpp"
#include "MResourceManager.hpp"
#include "MOpenCLManager.hpp"
#include "MSDLManager.hpp"
#include "DDMovie.hpp"
#include "FVMonoscopic.hpp"

/* temporary magick number solution */
#define CSTEREOSCOPIC_FRAME_SPAN 3
#define CSTEREOSCOPIC_CLASS_COUNT 16

CStereoscopic::CStereoscopic()
{

}

CStereoscopic::~CStereoscopic()
{

}

void CStereoscopic::Run()
{
	/* initialize managers */
	MLogManager* mLogManager = MLogManager::GetInstance();
	MMemoryManager* mMemoryManager = MMemoryManager::GetInstance();
	MResourceManager* mResourceManager = MResourceManager::GetInstance();
	MOpenCLManager* mOpenCLManager = MOpenCLManager::GetInstance();
	MSDLManager* mSDLManager = MSDLManager::GetInstance();

	mLogManager->Log(0,"[CStereoscopic.Run] Running");

	/* initialization */
	unsigned int iMovie = mResourceManager->LoadResourceFromFile("D:/Repositories/onroadlocalization/resources/3D STEREOSCOPIC ROAD-fMXW-4gmIwA.mp4");
	IResource* oMovie = mResourceManager->Get(iMovie);
	DDMovie* oDDMovie = (DDMovie*)oMovie->GetDataDescriptor();
	DDMovie* oDDSubMovie = new DDMovie(oDDMovie);

	mSDLManager->SetSize(oDDMovie->a_iWidth, oDDMovie->a_iHeight);

	unsigned int iProgram = mOpenCLManager->LoadProgramFromFile("D:/Repositories/onroadlocalization/opencl/stereoScopic.opencl",5);

	size_t iSizeFrame = oDDMovie->a_iHeight * oDDMovie->a_iWidth;
	unsigned int iBytesFrame = iSizeFrame * sizeof(glm::u8vec4);
	mLogManager->Info(0, "[CStereoscopic.Run] movie dimesions (%i,%i).", oDDMovie->a_iHeight, oDDMovie->a_iWidth);
	
	unsigned int iBytesFrameBuffer = CSTEREOSCOPIC_FRAME_SPAN * iBytesFrame;
	glm::u8vec4* oFrameBuffer = (glm::u8vec4*)mMemoryManager->Allocate(iBytesFrameBuffer);
	uint8_t* oFrame = (uint8_t*)oFrameBuffer;
	unsigned int iWriteIndex = 0;

	unsigned int iBytesFrameIndexMap = sizeof(unsigned int) * CSTEREOSCOPIC_FRAME_SPAN;
	unsigned int* liFrameIndexMap = (unsigned int*)mMemoryManager->Allocate(iBytesFrameIndexMap);

	unsigned int iBytesResults = sizeof(float) * CSTEREOSCOPIC_CLASS_COUNT;
	uint8_t* ldResults = (uint8_t*)mMemoryManager->Allocate(iBytesResults);
	
	unsigned int iBytesFreatureVector = sizeof(FVMonoscopic) * iSizeFrame;
	FVMonoscopic* loFreatureVector = (FVMonoscopic*)mMemoryManager->Allocate(sizeof(FVMonoscopic*) * oDDMovie->a_iHeight * oDDMovie->a_iWidth);

	/* set arguments */
	unsigned int eTypeReadPointer = OPENCL_MEM_READ_ONLY | OPENCL_MEM_COPY_HOST_PTR;
	
	mOpenCLManager->Use(iProgram);
	mOpenCLManager->SetChuncksTo(1, NULL, &iSizeFrame, NULL);
	mOpenCLManager->SetArgument(0, oFrameBuffer, iBytesFrameBuffer, false, false, eTypeReadPointer);
	mOpenCLManager->SetArgument(1, NULL, iBytesResults, false, true, OPENCL_MEM_WRITE_ONLY);
	mOpenCLManager->SetArgument(2, loFreatureVector, iBytesFreatureVector, false, false, OPENCL_MEM_COPY_HOST_PTR);
	mOpenCLManager->SetArgument(3, oDDSubMovie, sizeof(DDMovie), false, false, eTypeReadPointer);
	mOpenCLManager->SetArgument(4, liFrameIndexMap, iBytesFrameIndexMap, false, false, eTypeReadPointer);
	mOpenCLManager->UnUse();

	/* run */
	while (oMovie->GetData(oFrame)) {
		if (!mSDLManager->CheckEvent() && mSDLManager->IsExit()) { break; }

		/* put cap to frames fed to buffer and determine index map */
		oDDSubMovie->a_iFrames = (oDDMovie->a_iFrames >= CSTEREOSCOPIC_FRAME_SPAN ? CSTEREOSCOPIC_FRAME_SPAN : oDDMovie->a_iFrames);
		oDDSubMovie->a_iFrame = iWriteIndex;
		liFrameIndexMap[iWriteIndex] = iWriteIndex * iSizeFrame;

		/* compute and obtain results */
		mOpenCLManager->Use(iProgram);

		mOpenCLManager->Compute();
		mOpenCLManager->Result(1, ldResults);
		mOpenCLManager->Result(2, loFreatureVector);

		mOpenCLManager->Finish();
		mOpenCLManager->UnUse();

		/* visualize */
		mSDLManager->Display(oFrame);

		/* swap buffer */
		iWriteIndex = (oDDMovie->a_iFrames + 1) % CSTEREOSCOPIC_FRAME_SPAN;
		oFrame = (uint8_t*)oFrameBuffer + iWriteIndex * iBytesFrame;
	}

	/* clean up */
	mMemoryManager->DeAllocate(oFrameBuffer);
	mMemoryManager->DeAllocate(liFrameIndexMap);
	mMemoryManager->DeAllocate(loFreatureVector); 
	mMemoryManager->DeAllocate(ldResults);

	mLogManager->Log(0, "[CStereoscopic.Run] Stopped");
}