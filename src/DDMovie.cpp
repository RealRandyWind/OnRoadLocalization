#include "DDMovie.hpp"

DDMovie::DDMovie()
{
	DDMovie(0, 0, 0, 0, 0, 0.0, 1.0);
}

DDMovie::DDMovie(DDMovie* oDDMovie)
{
	a_iWidth = oDDMovie->a_iWidth;
	a_iHeight = oDDMovie->a_iHeight;
	a_iPitch = oDDMovie->a_iPitch;
	a_iFrame = oDDMovie->a_iFrame;
	a_iFrames = oDDMovie->a_iFrames;
	a_dFps = oDDMovie->a_dFps;
	a_dNorm = oDDMovie->a_dNorm;
}

DDMovie::DDMovie(unsigned int iWidth, unsigned int iHeight, unsigned int iPitch, unsigned int iFrame, unsigned int iFrames, float dFps, float dNorm)
{
	a_iWidth = iWidth;
	a_iHeight = iHeight;
	a_iPitch = iPitch;
	a_iFrame = iFrame;
	a_iFrames = iFrames;
	a_dFps = dFps;
	a_dNorm = dNorm;
}

DDMovie::~DDMovie()
{

}