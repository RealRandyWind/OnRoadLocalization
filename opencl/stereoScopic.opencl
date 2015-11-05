/*FORWARD DECLARETIONS*/

typedef struct _struct_datadescriptormovie DataDescriptorMovie;
typedef struct _struct_featurvector FeatureVector;



/*DEFINITIONS*/

struct _struct_datadescriptormovie 
{
	uint iWidth, iHeigth, iPitch, iFrame, iFrames, iSize;
	float dFps, dNorm;
};

/* maybe float16 works better computational wise. color 
is already stored in the lpData set. */
struct _struct_featurvector
{
	float4 vChange, vGradient;
	float3 vPosition, vVelocity, vNormal;
	float dAlpha;
};

/* LOCAL THREAD VARIABLE */
__constant float EPS = 1e-2;

/*FUNCTIONS*/

FeatureVector extractFeatures(
	__global read_only uchar4* lpData,
	__global read_only DataDescriptorMovie* oDescriptor,
	__global read_only uint* liFrameMap,
	uint iPixel)
{
	FeatureVector oFeatureVector;

	uint iWidth = oDescriptor->iWidth;
	uint iHeigth = oDescriptor->iHeigth;
	uint iFrame = oDescriptor->iFrame;
	uint iFrames = oDescriptor->iFrames;
	uint iIndexFrame = liFrameMap[iFrame];

	float4 pPixel =  convert_float4(lpData[iIndexFrame + iPixel]);

	/* extract position in 2d space */
	oFeatureVector.vPosition.x = (float)(iPixel % iWidth);
	oFeatureVector.vPosition.y = (float)(iPixel / iWidth);

	/* extract change in time duplicate on edge*/
	oFeatureVector.vChange = (
		( iFrames - iFrame ? convert_float4(lpData[iIndexFrame + 1 + iPixel]) : pPixel )
		- ( iFrame ? convert_float4(lpData[iIndexFrame - 1 + iPixel]) : pPixel )
	);

	/* extract velocity in 2d space */
	oFeatureVector.vVelocity.x = 0.0;
	oFeatureVector.vVelocity.y = 0.0;

	/*
	while(uint k=0; k<iFrames; ++k) {
		iIndexFrame = liFrameMap[k];
		uint i=0;
		uint j=0;
		while(uint i=0; i<iWidth; ++i)
		{
			while(uint j=0; j<iHeigth; ++j)
			{
				uint iIndex = j * iWidth + j;
			}
		} 
	}
	*/

	return oFeatureVector;
}

/*PROGRAM ENTRY*/

__kernel void entry(
	__global read_only uchar4* lpData,
	__global write_only float* ldResults,
	__global FeatureVector* loFeatures,
	__global read_only DataDescriptorMovie* oDescriptor,
	__global read_only uint* liFrameMap)
{
	/* each thread processes one pixel in the current frame*/

	/* determine thread id */
	size_t iThread = get_global_id(0);

	/* extracting freatures from frame per pixel*/
	loFeatures[iThread] = extractFeatures(lpData, oDescriptor, liFrameMap, iThread);
}