#include "CMonoscopic.hpp"
#include "MLogManager.hpp"
#include "MMemoryManager.hpp"
#include "MResourceManager.hpp"
#include "MOpenCLManager.hpp"
#include "MSDLManager.hpp"
#include "DDMovie.hpp"

/* computation monoscopic default settings */
#define CMONOSCOPIC_FRAME_SPAN 3
#define CMONOSCOPIC_CLASS_COUNT 16
#define CMONOSCOPIC_FEATURE_COUNT 16
#define CMONOSCOPIC_CHUNK_DIMENSIONS 1
#define CMONOSCOPIC_DIFFERENCE_EPS 5.0f
#define CMONOSCOPIC_ARGUMENT_COUNT 9

/* computation monoscopic feature vector indices */
#define CMFV_DTR 0
#define CMFV_DTG 1
#define CMFV_DTB 2
#define CMFV_DTA 3
#define CMFV_DIR 4
#define CMFV_DIG 5
#define CMFV_DIB 6
#define CMFV_DIA 7
#define CMFV_PIX 8
#define CMFV_PIY 9
#define CMFV_GTI 10
#define CMFV_LAST 10

/* computation monoscopic argument indices */
#define CMARG_DATA 0
#define CMARG_RSLT 1
#define CMARG_FVEC 2
#define CMARG_DESC 3
#define CMARG_FMAP 4
#define CMARG_KRNS 5
#define CMARG_KRNL 6
#define CMARG_DEPS 7
#define CMARG_DISP 8

/* computation monoscopic kernels and info */
#define CMKERNELS_SOBEL { -1.0f, 0.0f, 1.0f, -2.0f, 0.0f, 2.0f, -1.0f, 0.0f, 1.0f, -1.0f, -2.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f, 1.0f, 0.0f, 0.0f, 0.0f }
#define CMKERNELS_SOBEL_TIME { -1.0f, 0.0f, 1.0f, -2.0f, 0.0f, 2.0f, -1.0f, 0.0f, 1.0f, -1.0f, -2.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f, 1.0f, -1.0f, 0.0f, 1.0f }
#define CMKERNELS_SOBEL_SIZES { 9, 3 }
#define CMKERNELS_SOBEL_ELEMENT_COUNT 21

CMonoscopic::CMonoscopic()
{

}

CMonoscopic::~CMonoscopic()
{

}

void CMonoscopic::Run()
{
	/* initialize managers */
	MLogManager* mLogManager = MLogManager::GetInstance();
	MMemoryManager* mMemoryManager = MMemoryManager::GetInstance();
	MResourceManager* mResourceManager = MResourceManager::GetInstance();
	MOpenCLManager* mOpenCLManager = MOpenCLManager::GetInstance();
	MSDLManager* mSDLManager = MSDLManager::GetInstance();

	mLogManager->Log(0, "[CMonoscopic.Run] Running");

	/* set helper variables */
	unsigned int eTypeReadPointer = OPENCL_MEM_READ_ONLY | OPENCL_MEM_COPY_HOST_PTR;
	unsigned int iWriteIndex = CMONOSCOPIC_FRAME_SPAN - 1;

	/* initialization movie */
	unsigned int iMovie = mResourceManager->LoadResourceFromFile("C:/Repositories/onroadlocalization/resources/Scotland Scenic Drives 42.  Stranraer to Cairnryan (via A77)-JWys4j1BjC4.mp4");
	IResource* oMovie = mResourceManager->Get(iMovie);
	DDMovie* oDDMovie = (DDMovie*)oMovie->GetDataDescriptor();
	DDMovie* oDDSubMovie = new DDMovie(oDDMovie);
	oDDSubMovie->a_iMaxFrames = CMONOSCOPIC_FRAME_SPAN;

	mSDLManager->SetSize(oDDMovie->a_iWidth, oDDMovie->a_iHeight);

	/* initialization opencl and its arguments */
	unsigned int iProgram = mOpenCLManager->LoadProgramFromFile("C:/Repositories/onroadlocalization/opencl/monoScopic.opencl", CMONOSCOPIC_ARGUMENT_COUNT);

	size_t iSizeFrame = oDDMovie->a_iHeight * oDDMovie->a_iWidth;
	unsigned int iBytesFrame = iSizeFrame * sizeof(glm::u8vec4);
	uint8_t* oDisplay = (uint8_t*)mMemoryManager->Allocate(iBytesFrame);
	mLogManager->Info(0, "[CMonoscopic.Run] movie dimesions and fps (%i,%i,%f).", oDDMovie->a_iHeight, oDDMovie->a_iWidth, oDDMovie->a_dFps);
	
	unsigned int iBytesFrameBuffer = CMONOSCOPIC_FRAME_SPAN * iBytesFrame;
	glm::u8vec4* oFrameBuffer = (glm::u8vec4*)mMemoryManager->Allocate(iBytesFrameBuffer);
	uint8_t* oFrame = (uint8_t*)oFrameBuffer;

	unsigned int iBytesFrameIndexMap = sizeof(unsigned int) * CMONOSCOPIC_FRAME_SPAN;
	unsigned int* liFrameIndexMap = (unsigned int*)mMemoryManager->Allocate(iBytesFrameIndexMap);

	unsigned int iBytesResults = sizeof(float) * CMONOSCOPIC_CLASS_COUNT;
	uint8_t* ldResults = (uint8_t*)mMemoryManager->Allocate(iBytesResults);
	
	unsigned int iBytesFreatureVector = sizeof(float) * CMONOSCOPIC_FEATURE_COUNT * iSizeFrame;
	float* ldFreatureVector = (float*)mMemoryManager->Allocate(iBytesFreatureVector);
		
		/* temporary solution */
	unsigned int iBytesKernels = sizeof(float) * CMKERNELS_SOBEL_ELEMENT_COUNT;
	float ldKernels[] = CMKERNELS_SOBEL;
		/* temporary solution */
	unsigned int iBytesKernelLenghts = sizeof(unsigned int) * 2;
	unsigned int liKernelLenghts[] = CMKERNELS_SOBEL_SIZES;

	float dDiffEps = CMONOSCOPIC_DIFFERENCE_EPS;
	
	/* select program and chuncksize */
	mOpenCLManager->Use(iProgram);
	mOpenCLManager->SetChuncksTo(CMONOSCOPIC_CHUNK_DIMENSIONS, NULL, &iSizeFrame, NULL);

	/* set result and static arguments */
	mOpenCLManager->SetArgument(CMARG_RSLT, NULL, iBytesResults, false, true, OPENCL_MEM_WRITE_ONLY);
	mOpenCLManager->SetArgument(CMARG_FVEC, NULL, iBytesFreatureVector, false, true, OPENCL_MEM_WRITE_ONLY);
	mOpenCLManager->SetArgument(CMARG_KRNS, &ldKernels, iBytesKernels, false, false, eTypeReadPointer);
	mOpenCLManager->SetArgument(CMARG_KRNL, &liKernelLenghts, iBytesKernelLenghts, false, false, eTypeReadPointer);
	mOpenCLManager->SetArgument(CMARG_DEPS, &dDiffEps, sizeof(float), true, false, OPENCL_MEM_READ_ONLY);
	mOpenCLManager->SetArgument(CMARG_DISP, NULL, iBytesFrame, false, true, OPENCL_MEM_WRITE_ONLY);
	
	mOpenCLManager->UnUse();
	
	/* DEBUG logging */
	mLogManager->Debug(0, "[CMonoscopic.Run] M(%i,%i,%i) -> (float,uint,char)", sizeof(float), sizeof(unsigned int), sizeof(char));
	mLogManager->Debug(0, "[CMonoscopic.Run] C(%i,%i,%i) -> (float,uint,char)", sizeof(cl_float), sizeof(cl_uint), sizeof(cl_char));
	mLogManager->Debug(0, "[CMonoscopic.Run] IDataDescriptor:(%i), DDMovie:(%i,%i) -> (M,C)", sizeof(IDataDescriptor), sizeof(DDMovie)-sizeof(IDataDescriptor), 5 * sizeof(cl_uint) + 2 * sizeof(cl_float) + 1*sizeof(cl_float));

	/* run */
	while (oMovie->GetData(oFrame)) {
		if (!mSDLManager->CheckEvent() && mSDLManager->IsExit()) { break; }

		/* put cap to frames fed to buffer and determine index map */
		oDDSubMovie->a_iFrames = (oDDMovie->a_iFrames >= CMONOSCOPIC_FRAME_SPAN ? CMONOSCOPIC_FRAME_SPAN : oDDMovie->a_iFrames);
		oDDSubMovie->a_iFrame = iWriteIndex;
		liFrameIndexMap[iWriteIndex] = iWriteIndex * iSizeFrame;

		mOpenCLManager->Use(iProgram);

		/* set variable arguments */
		mOpenCLManager->SetArgument(CMARG_DATA, oFrameBuffer, iBytesFrameBuffer, false, false, eTypeReadPointer);
		mOpenCLManager->SetArgument(CMARG_DESC, oDDSubMovie, sizeof(DDMovie), false, false, eTypeReadPointer);
		mOpenCLManager->SetArgument(CMARG_FMAP, liFrameIndexMap, iBytesFrameIndexMap, false, false, eTypeReadPointer);
		
		/* compute */
		mOpenCLManager->Compute();

		/* obtain results */
		mOpenCLManager->Result(CMARG_RSLT, ldResults);
		mOpenCLManager->Result(CMARG_FVEC, ldFreatureVector);
		mOpenCLManager->Result(CMARG_DISP, oDisplay);

		mOpenCLManager->Finish();
		mOpenCLManager->UnUse();

		/* visualize */
		mSDLManager->Display(oDisplay);

		/* swap buffer */
		iWriteIndex = CMONOSCOPIC_FRAME_SPAN - (oDDMovie->a_iFrames % CMONOSCOPIC_FRAME_SPAN) - 1;
		oFrame = (uint8_t*)oFrameBuffer + iWriteIndex * iBytesFrame;
	}

	/* clean up */
	mMemoryManager->DeAllocate(oFrameBuffer);
	mMemoryManager->DeAllocate(liFrameIndexMap);
	mMemoryManager->DeAllocate(ldFreatureVector);
	mMemoryManager->DeAllocate(ldResults);
	mMemoryManager->DeAllocate(oDisplay);

	mLogManager->Log(0, "[CMonoscopic.Run] Stopped");
}