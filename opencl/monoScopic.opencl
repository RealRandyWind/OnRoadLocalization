/*FORWARD DECLARETIONS*/

typedef struct _struct_datadescriptormovie DataDescriptorMovie;
typedef struct _struct_featurvector FeatureVector;


/*DEFINITIONS*/

struct _struct_datadescriptormovie 
{
	float _vptr[2];
	uint iWidth, iHeigth, iPitch, iFrame, iFrames, iMaxFrames;
	float dFps, dNorm;
};

/* maybe float8 works better computational wise. color 
is already stored in the lpData set. */
struct _struct_featurvector
{
	float4 vDT,vDI;
	float2 vPI;
	float dGTI;
	float _padding[5];
};

/* LOCAL THREAD VARIABLE */
__constant float EPS = 1e-2f;
__constant float UCNORM = 255;

/*FUNCTIONS*/

FeatureVector extractFeatures(
	__global read_only uchar4* lpData,
	__global read_only DataDescriptorMovie* oDescriptor,
	__global read_only uint* liFrameMap,
	uint iPixel,
	float dEps,
	__global write_only uchar4* lpDisplay)
{
	FeatureVector oFeatureVector;

	/* set variable for easy access */
	uint iWidth = oDescriptor->iWidth;
	uint iHeigth = oDescriptor->iHeigth;
	uint iFrame = oDescriptor->iFrame;
	uint iFrames = oDescriptor->iFrames;
	uint iMaxFrames = oDescriptor->iMaxFrames;
	uint iIndexFrame = liFrameMap[iFrame];
	uchar4 pPixel = lpData[iIndexFrame + iPixel];
	float4 vPixel = convert_float4(pPixel)/UCNORM;

	/* method for hisory frames access */
	uint iPreviousFrame = ((iFrame + 1) % iFrames) + (iMaxFrames - iFrames);
	uint iPreviousIndexFrame = liFrameMap[iPreviousFrame];
	uchar4 pPreviousPixel = lpData[iPreviousIndexFrame + iPixel];

	/* extract position in image */
	uint iX = (iPixel % iWidth);
	uint iY = (iPixel / iWidth);
	oFeatureVector.vPI.x = (float)iX;
	oFeatureVector.vPI.y = (float)iY;

	/* differenciate in time */

	/* differenciate in image */

	/* gradient over time and image */
	float dGx = 0.0f;
	float dGy = 0.0f;
	float dGt = 0.0f;
	

	oFeatureVector.dGTI = 0.0f;

	/* update display pixel */
	lpDisplay[iPixel] = pPreviousPixel - pPixel;

	return oFeatureVector;
}

/*PROGRAM ENTRY*/

__kernel void entry(
	__global read_only uchar4* lpData,
	__global write_only float* ldResults,
	__global write_only FeatureVector* loFeatures,
	__global read_only DataDescriptorMovie* oDescriptor,
	__global read_only uint* liFrameMap,
	float dEps,
	__global write_only uchar4* lpDisplay)
{
	/* each thread processes one pixel in the current frame*/

	/* determine thread id */
	size_t iThread = get_global_id(0);

	/* extracting freatures from frame per pixel*/
	loFeatures[iThread] = extractFeatures(
		lpData,
		oDescriptor,
		liFrameMap,
		iThread,
		dEps,
		lpDisplay
	);
}
