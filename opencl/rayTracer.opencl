/* STRUCT DEFINITIONS */
enum EObjectType {
	SPHERE=0,
	PLANE=1
};

enum ETraceMode {
	TRACE = 1<<1,
	REFLECTION = 1<<2,
	TRANSPARENCY = 1<<3,
	FRESNEL = 1<<4 | TRANSPARENCY
};

typedef struct _struct_object Object;
struct _struct_object {
	uint iObject, eType, iMaterial, iData, nData;
	float4 pOrigin, vNormal;
	float dRadius, dTheta;
	float4 mModelMatrix[4];
};

typedef struct _struct_light Light;
struct _struct_light {
	uint iLight, eType, iData, nData;
	float4 pOrigin, vDirection;
	float4 pColor;
};

typedef struct _struct_material Material;
struct _struct_material {
	uint iMaterial, iIlluminationModel, iSharpness, iNs;
	float4 vKa, vKd, vKs, vTf;
	float dDHalo, dNi;
};

typedef struct _struct_texture Texture;
struct _struct_texture {
	uint iTexture, eType, iData;
	uint iDimensions[5];
};

typedef struct _struct_ray Ray;
struct _struct_ray {
	float4 pOrigin, vDirection;
};

typedef struct _struct_hit Hit;
struct _struct_hit {
	float4 pOrigin, vNormal;
	float dT, dLenght;
	uint iDepth;
	__global read_only Object* oObject;
};

typedef struct _struct_camera Camera;
struct _struct_camera {
	uint iCamera;
	float4 pOrigin, vUp, vRight, vFront;
	float dFocalDistance, dAspectRatio, dFovY, dNear, dFar, dLambda;
	float4 mProjectionMatrix[4];
	float4 mViewMatrix[4];
};

typedef struct _struct_scene Scene;
struct _struct_scene {
	float4 pAmbiantColor, pClearColor;
	uint nObjects, nLights, nMaterials, nTextures;
	__global read_only Camera* oCamera;
	__global read_only Object* aObjects;
	__global read_only Light* aLights;
	__global read_only Material* aMaterials;
	__global read_only Texture* aTextures;
	__global read_only float* aData;
	/*__global read_only Data* aData;*/
};

typedef Object Sphere;
typedef Object Plane;

/* LOCAL THREAD VARIABLE */
__constant float EPS = 1e-2;

/* HELPER FUNCTIONS */
Hit noHit()
{
	Hit oHit;
	oHit.oObject = 0;
	oHit.dT = FLT_MAX;
	return oHit;
}

/* INTERSECT FUNCTIONS*/
Hit interstectSphere(Ray *oRay, __global read_only Sphere *oObject)
{
	float dA,dB,dC,dD,dSqrtD,dT1,dT2,dT,dR;
	
	float4 vVec = oRay->pOrigin-oObject->pOrigin;
	
	dA = dot(oRay->vDirection,oRay->vDirection);
    dB = dot(oRay->vDirection,vVec);
    dC = dot(vVec,vVec);
    dR = oObject->dRadius;
    dD = dB*dB-dA*(dC-dR*dR);

    if(dD<0){ return noHit(); }

    if(dD>0) { 
		dSqrtD = sqrt(dD);
		dT1 = -(dB + dSqrtD)/dA;
    	dT2 = -(dB - dSqrtD)/dA;
	} else { dT1 = dT2 = -(dB + sqrt(dD))/dA; }

	dT = (dT1 < dT2 ? (dT1 <= 0 ? dT2 : dT1) : dT2);

	if(dT<=0) { return noHit(); }

	Hit oHit;
	oHit.dT = dT;
	oHit.dLenght = dT;
	oHit.pOrigin = oRay->pOrigin + dT*oRay->vDirection;
	oHit.vNormal = normalize(oHit.pOrigin - oObject->pOrigin);
	oHit.oObject = oObject;
	oHit.iDepth = 0;

	return oHit;
}

Hit interstectPlane(Ray *oRay, __global read_only Plane *oObject)
{
	float dT = dot(oObject->pOrigin - oRay->pOrigin,oObject->vNormal)/dot(oRay->vDirection,oObject->vNormal);
	if(dT<=0) { return noHit(); }

	Hit oHit;
	oHit.dT = dT;
	oHit.dLenght = dT;
	oHit.pOrigin = oRay->pOrigin + dT*oRay->vDirection;
	oHit.vNormal = oObject->vNormal;
	oHit.oObject = oObject;
	oHit.iDepth = 0;

	return oHit;
}

Hit intersect(Ray *oRay, __global read_only Object* oObject)
{
	switch(oObject->eType){
		case SPHERE : return interstectSphere(oRay,oObject);
		case PLANE : return interstectPlane(oRay,oObject);
		default : break;
	}
	return  noHit();
}

Hit findIntersect(Ray *oRay, __global read_only Object* aObjects, uint nObjects)
{
	Hit oMinHit = noHit();
	while(nObjects--){
		Hit oHit = intersect(oRay,&aObjects[nObjects]);
		if(oHit.dT < oMinHit.dT) { oMinHit = oHit; }
	}
	return oMinHit;
}

Hit findShadow(float4 pOrigin, float4 vDirection, Scene *oScene)
{
	Ray oRay;
	oRay.pOrigin = pOrigin;
	oRay.vDirection = vDirection;
	return findIntersect(&oRay,oScene->aObjects,oScene->nObjects);
}

/* TRACE FUNCTIONS */
float4 illuminate(Ray *oRay, Hit *oHit, float4 *vFactor, Scene *oScene, uint* eMode)
{
	(*eMode) = 0;
	if(!oHit->oObject) { return (float4)(0,0,0,0); }
	
	__global read_only Material* oMaterial = &oScene->aMaterials[oHit->oObject->iMaterial];

	if(!oMaterial->iIlluminationModel) { return oMaterial->vKd; }

	float4 pAmbiant = oScene->pAmbiantColor;
	float4 V = oRay->vDirection;
	float4 N = oHit->vNormal;
	float4 pRayHitError = (oHit->dT - EPS) * V;

	float dDissolve = 1.0;

	float4 pDiffuse = (float4)(0,0,0,0);
	float4 pSpecular = (float4)(0,0,0,0);
	float4 pFresnel = (float4)(0,0,0,0);

	switch(oMaterial->iIlluminationModel){
		case 1: { 
			for(int i=0; i<oScene->nLights; ++i)
			{
				__global read_only Light* oLight = &oScene->aLights[i];
				float4 L = normalize(oLight->pOrigin - oHit->pOrigin);
				Hit oShadowHit = findShadow(pRayHitError, L, oScene);
				if(oShadowHit.oObject) { continue; }
				float4 H = normalize(L+V);
				pDiffuse += max(dot(N,L),0.0) * oLight->pColor;
			}
			return oMaterial->vKa * pAmbiant + oMaterial->vKd * pDiffuse;
		}
		case 3: { (*eMode) |= TRACE | REFLECTION; }
		case 2: { 
			for(int i=0; i<oScene->nLights; ++i)
			{
				__global read_only Light* oLight = &oScene->aLights[i];
				float4 L = normalize(oLight->pOrigin - oHit->pOrigin);
				Hit oShadowHit = findShadow(pRayHitError, L, oScene);
				if(oShadowHit.oObject) { continue; }
				float4 H = normalize(L+V);
				float dNDotL = max(dot(N,L),0.0);
				float4 R = L - 2.0 * dNDotL * N;
				pDiffuse += dNDotL * oLight->pColor;
				pSpecular += pow(dot(R,V),oMaterial->iNs) * oLight->pColor;
			}
			(*vFactor) *= oMaterial->vKs;
			return oMaterial->vKa * pAmbiant + oMaterial->vKd * pDiffuse + oMaterial->vKs * pSpecular;
		}
		default : return (float4)(0,0,1,0);
	}

	return (float4)(1,0,0,0);
}

float4 trace(Ray *oSourceRay, Scene *oScene, uint iMaxRecursionDepth)
{
	float dFar = oScene->oCamera->dFar;
	uint eMode = TRACE;
	
	float4 pColor = (float4)(0,0,0,0);
	float4 vFactor = (float4)(1,1,1,1);
	float4 vLastFactor = vFactor;
	
	Ray oRay = (*oSourceRay);
	
	Hit oHit = findIntersect(&oRay,oScene->aObjects,oScene->nObjects);
	if(!oHit.oObject) { return pColor; }
	Hit oLastHit = oHit;

	while((eMode & TRACE) && oLastHit.iDepth<iMaxRecursionDepth && oLastHit.dLenght<=dFar){
		Hit oHit = findIntersect(&oRay,oScene->aObjects,oScene->nObjects);
		if(!oHit.oObject) { return pColor; }
		oHit.dLenght += oLastHit.dLenght;
		oHit.iDepth = oLastHit.iDepth + 1;
		
		pColor += vFactor*illuminate(&oRay,&oHit,&vFactor,oScene,&eMode);

		float4 V = oRay.vDirection;
		float4 N = oHit.vNormal;
		oRay.pOrigin = (oHit.dT - EPS) * V;
		oRay.vDirection = V - 2.0 * dot(V,N) * N;

		vLastFactor = vFactor;
		oLastHit = oHit;
	}
	return pColor;
}

/* ENTRY POINT*/
__kernel void entry(
	__global write_only float4* pPixel, 
	__global read_only Camera* oCamera, 
	__global read_only Object* aObjects, uint nObjects, 
	__global read_only Light* aLights, uint nLights, 
	__global read_only Material* aMaterials, uint nMaterials,
	__global read_only Texture* aTextures, uint nTextures,
	uint iWidth, uint iHeight, uint iSuperSampling, 
	uint iMaxRecursionDepth, float4 pAmbiantColor, float4 pClearColor,
	__global read_only float* aData)
{
	/* make scene information passable*/
	Scene oScene;
	oScene.pAmbiantColor = pAmbiantColor;
	oScene.pClearColor = pClearColor;
	oScene.oCamera = oCamera;
	oScene.aObjects = aObjects;
	oScene.aLights = aLights;
	oScene.aMaterials = aMaterials;
	oScene.aTextures = aTextures;
	oScene.nObjects = nObjects;
	oScene.nLights = nLights;
	oScene.nMaterials = nMaterials;
	oScene.nTextures = nTextures;

	/* determine thread and workgroup */ 	
	size_t iThread = get_global_id(0);
	
	/* set background color */
	float4 pColor = pClearColor;

	/* computer pixel point in space*/
	float dX = (float)(iThread%iWidth);
	float dY = (float)(iThread/iWidth);
	float dDXnY = (oCamera->dLambda/iHeight);
	float dXPrime = dX*oCamera->dLambda/(float)iHeight - ((float)iWidth*0.5)*oCamera->dLambda/(float)iHeight;
	float dYPrime = dY*oCamera->dLambda/(float)iHeight - ((float)iHeight*0.5)*oCamera->dLambda/(float)iHeight;
	float4 pPoint = dXPrime * oCamera->vRight + dYPrime * oCamera->vUp + oCamera->dNear * oCamera->vFront;
	
	/* construct ray */
	Ray oRay;
	oRay.pOrigin = oCamera->pOrigin;
	oRay.vDirection = normalize(pPoint - oCamera->pOrigin);
	
	/* default trace */
	pColor += clamp(0,1,trace(&oRay,&oScene,iMaxRecursionDepth));
	pPixel[iThread] = pColor;
	
	/* add supersampling */
	float dSuperSampling2 = (iSuperSampling*iSuperSampling);
	float dDXnYss = (1/iSuperSampling);
	
	for(uint i=0;i<iSuperSampling;++i){
		for(uint j=0;j<iSuperSampling;++j){
			dXPrime = (dX + (1/(float)iSuperSampling)*(float)i - 0.5f)*(oCamera->dLambda/(float)iHeight) - ((float)iWidth*0.5)*(oCamera->dLambda/(float)iHeight);
			dYPrime = (dY + (1/(float)iSuperSampling)*(float)j - 0.5f)*(oCamera->dLambda/(float)iHeight) - ((float)iHeight*0.5)*(oCamera->dLambda/(float)iHeight);
			pPoint = dXPrime * oCamera->vRight + dYPrime * oCamera->vUp + oCamera->dNear * oCamera->vFront;
			oRay.vDirection = normalize(pPoint - oCamera->pOrigin);
			pColor += clamp(0,1,trace(&oRay,&oScene,iMaxRecursionDepth));
		}
	}

	/* update color */
	pPixel[iThread] = (iSuperSampling>0 ? pColor/(iSuperSampling*iSuperSampling+1) : pColor);
	
}