#ifndef RMOVIE_HPP
#define RMOVIE_HPP
#include "IResource.hpp"
#include "IDataDescriptor.hpp"
#include "MLogManager.hpp"
#include "MMemoryManager.hpp"
#include "DDMovie.hpp"
#include <string>
extern "C" {
	#include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	#include "libavutil/opt.h"
	#include "libswscale/swscale.h"
}

class RMovie;
class RMovie : public IResource
{
private:
	MLogManager* a_mLogManager;
	MMemoryManager* a_mMemoryManager;

	size_t a_iSize;

	AVFormatContext* a_oFContext;
	AVCodecContext* a_oCContext;
	SwsContext* a_oSContext;
	AVFrame* a_oFrame = NULL;
	AVCodec* a_oCodec = NULL;
	int a_iStreamIndex;
	int a_iFrameFinished;

	AVPacket a_sPacket;

	DDMovie* a_oDescriptor;
	
	void Fx_LoadMovie(std::string sPath);
	uint8_t* Fx_NextFrame(uint8_t* oDestination);
public:
	RMovie(std::string sPath);
	~RMovie();

	virtual uint8_t* GetData(uint8_t* oDestination=NULL);
	virtual void Display(void** oDestination, int iPitch);
	virtual IDataDescriptor* GetDataDescriptor();
};
#endif