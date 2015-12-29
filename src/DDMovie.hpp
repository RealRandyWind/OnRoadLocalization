#ifndef DDMOVIE_HPP
#define DDMOVIE_HPP
#include "IDataDescriptor.hpp"

class DDMovie;
class DDMovie : public IDataDescriptor
{
public:
	unsigned int a_iWidth;
	unsigned int a_iHeight;
	unsigned int a_iPitch;
	unsigned int a_iFrame;
	unsigned int a_iFrames;
	unsigned int a_iMaxFrames;
	float a_dFps;
	float a_dNorm;
	
	DDMovie();
	DDMovie(DDMovie* oDDMovie);
	DDMovie(unsigned int iWidth, unsigned int iHeight, unsigned int iPitch, unsigned int iFrame, unsigned int iFrames, float dFps, float dNorm);
	~DDMovie();
};
#endif