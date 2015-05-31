#ifndef RMOVIE_HPP
#define RMOVIE_HPP
#include "IResource.hpp"
#include <string>
extern "C" {
	#include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	#include "libavutil/opt.h"
}

class RMovie;
class RMovie : public IResource
{
private:
	void* a_pData;
	size_t iSize;

	void* Fx_LoadMovie(std::string sPath);
	void* Fx_NextFrame(AVFormatContext* oFContext, AVCodecContext* oCContext, int iStreamIndex);
public:
	RMovie(std::string sPath);
	~RMovie();

	virtual void* GetData();
	virtual void* GetData(void* pAt, unsigned int iCount=1);
	virtual void* GetRangeData(void* pFrom, void* pTo, unsigned int iCount=1);
	virtual void* GetDataDescriptor();
};
#endif