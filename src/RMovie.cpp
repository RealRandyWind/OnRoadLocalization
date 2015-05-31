#include "RMovie.hpp"
#include "MLogManager.hpp"

RMovie::RMovie(std::string sPath)
{
	iSize = -1;
	a_pData = RMovie::Fx_LoadMovie(sPath);
}

RMovie::~RMovie()
{

}

void* RMovie::Fx_LoadMovie(std::string sPath)
{
	MLogManager* mLogManager = MLogManager::GetInstance();
	
	av_register_all();
	
	int iStreamIndex = -1;
	AVFormatContext *oFContext = NULL;
	AVCodecContext *oCContext = NULL;
	AVCodec *oCodec = NULL;

	if (avformat_open_input(&oFContext, sPath.c_str(), NULL, NULL) < 0) {
		mLogManager->LogF(LOG_ERROR, 0, "RMovie could not open file \"%s\"", sPath.c_str());
		return NULL;
	}

	if (avformat_find_stream_info(oFContext, NULL) < 0) {
		mLogManager->LogF(LOG_ERROR, 0, "RMovie could not find stream information at url \"%s\"", sPath.c_str());
		return NULL;
	}

	if (iStreamIndex = av_find_best_stream(oFContext, AVMEDIA_TYPE_VIDEO, -1, -1, &oCodec, 0) < 0) {
		mLogManager->LogF(LOG_ERROR, 0, "RMovie could not find a video stream from url/file \"%s\"", sPath.c_str());
		return NULL;
	}
	oCContext = oFContext->streams[iStreamIndex]->codec;
	av_opt_set_int(oCContext, "refcounted_frames", 1, 0);

	_ASSERTE(oCodec != NULL);

	if (avcodec_open2(oCContext, oCodec, NULL) < 0) {
		mLogManager->LogF(LOG_ERROR, 0, "RMovie could not open video decoder");
		return NULL;
	}

	mLogManager->LogF(LOG_SUCCESS, 0, "RMovie \"%s\" loaded", sPath.c_str());
	
	// TODO different place
	Fx_NextFrame(oFContext, oCContext, iStreamIndex);

	avcodec_close(oCContext);
	avformat_close_input(&oFContext);

	return NULL;
}

void* RMovie::Fx_NextFrame(AVFormatContext* oFContext, AVCodecContext* oCContext, int iStreamIndex)
{
	MLogManager* mLogManager = MLogManager::GetInstance();

	int iFrameFinished = 0;
	int nFrames = 0;
	uint8_t* aBuffer = NULL;
	AVPacket oPacket;
	AVFrame* oFrame = av_frame_alloc();

	while (av_read_frame(oFContext, &oPacket) >= 0) {

		if (oPacket.stream_index == iStreamIndex) {
			if (avcodec_decode_video2(oCContext, oFrame, &iFrameFinished, &oPacket) < 0) {
				mLogManager->LogF(LOG_ERROR, 0, "RMovie could not decode frame");
			}
			++nFrames;
		}

		av_free_packet(&oPacket);
	}
	
	av_free(oFrame);

	mLogManager->LogF(LOG_INFO, 0, "RMovie read %i frames",nFrames);

	return NULL;
}

void* RMovie::GetData()
{
	return a_pData;
}

void* RMovie::GetData(void* pAt, unsigned int iCount)
{
	return NULL;
}

void* RMovie::GetRangeData(void* pFrom, void* pTo, unsigned int iCount)
{
	return NULL;
}

void* RMovie::GetDataDescriptor()
{
	return NULL;
}