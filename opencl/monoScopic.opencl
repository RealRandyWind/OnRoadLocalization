/*FORWARD DECLARETIONS*/

typedef struct _struct_datadescriptormovie DataDescriptorMovie;
typedef struct _struct_featurvector FeatureVector;



/*DEFINITIONS*/

struct _struct_datadescriptormovie 
{
	uint iFrameFrom, iFrameTo, iWidth, iHeigth;
	float dFps, dMax, dMin, dEpsilon;
};


/* maybe float8 works better computational wise. color 
is already stored in the lpData set. */
struct _struct_featurvector
{
	float2 vPosition, vVelocity, vNormal;
}



/*FUNCTIONS*/

FeatureVector extractFeatures(
	__global read_only float4* lpData,
	__global read_only DataDescriptorMovie* oDescriptor,
	uint iPixel)
{
	FeatureVector oFeatureVector;

	/* extract position */
	oFeatureVector.vPosition.x = (float)(iThread % oDescriptor.iWidth);
	oFeatureVector.vPosition.y = (float)(iThread / oDescriptor.iWidth);

	oFeatureVector.vVelocity.x = 0.0;
	oFeatureVector.vVelocity.y = 0.0;

	oFeatureVector.vNormal.x = 0.0;
	oFeatureVector.vNormal.y = 0.0;
	

	oFeatureVector.dLifetime = oPredecessor;
	oFeatureVector.dDeviation = extractDeviation();

	return oFeatureVector;
}



/*PROGRAM ENTRY*/

__kernel void main(
	__global read_only float4* lpData,
	__global write_only float* ldResults,
	__global FeatureVector* loFeatures,
	__global read_only DataDescriptorMovie* oDescriptor,
	uint iFrameCurrent)
{
	/* for now each thread processes one pixel in the current frame*/

	/* determine thread id */ 	
	size_t iThread = get_global_id(0);

	/* extracting freatures from frame per pixel*/
	loFeatures[iThread] = extractFeatures(lpData,iThread);
}