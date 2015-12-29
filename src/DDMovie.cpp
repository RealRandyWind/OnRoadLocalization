#include "DDMovie.hpp"

DDMovie::DDMovie()
{
	DDMovie(0, 0, 0, 0, 0, 0.0, 1.0);
}

DDMovie::DDMovie(DDMovie* oDDMovie)
{
	DDMovie(oDDMovie->a_iWidth, oDDMovie->a_iHeight, oDDMovie->a_iPitch, oDDMovie->a_iFrame, oDDMovie->a_iFrames, oDDMovie->a_dFps, oDDMovie->a_dNorm);
}

DDMovie::DDMovie(unsigned int iWidth, unsigned int iHeight, unsigned int iPitch, unsigned int iFrame, unsigned int iFrames, float dFps, float dNorm)
{
	a_iWidth = iWidth;
	a_iHeight = iHeight;
	a_iPitch = iPitch;
	a_iFrame = iFrame;
	a_iFrames = iFrames;
	a_iMaxFrames = 0;
	a_dFps = dFps;
	a_dNorm = dNorm;
}

DDMovie::~DDMovie()
{

}