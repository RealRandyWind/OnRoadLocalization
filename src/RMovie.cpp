#include "RMovie.hpp"

RMovie::RMovie(std::string sPath)
{
	a_mLogManager = MLogManager::GetInstance();
	a_mMemoryManager = MMemoryManager::GetInstance();

	a_oFContext = NULL;
	a_oCContext = NULL;
	a_oCodec = NULL;
	a_oSContext = NULL;
	a_oFrame = av_frame_alloc();;

	a_oDescriptor = NULL;
	
	a_iSize = -1;
	a_iStreamIndex = -1;
	a_iFrameFinished = 0;
	
	RMovie::Fx_LoadMovie(sPath);
}

RMovie::~RMovie()
{
	if (a_iFrameFinished) { av_free_packet(&a_sPacket); }

	if (a_oFrame) {
		av_free(a_oFrame);
	}

	if (a_oSContext) {
		sws_freeContext(a_oSContext);
	}
	
	if (a_oCContext) {
		avcodec_close(a_oCContext);
	}

	if (a_oFContext) {
		avformat_close_input(&a_oFContext);
	}
}

void RMovie::Fx_LoadMovie(std::string sPath)
{	
	av_register_all();
	avformat_network_init();

	if (avformat_open_input(&a_oFContext, sPath.c_str(), NULL, NULL) < 0) {
		a_mLogManager->Warning(0, "[RMovie.Fx_LoadMovie] could not open file \"%s\".", sPath.c_str());
		return;
	}

	if (avformat_find_stream_info(a_oFContext, NULL) < 0) {
		a_mLogManager->Warning(0, "[RMovie.Fx_LoadMovie] could not find stream information at url \"%s\".", sPath.c_str());
		return;
	}

	if (a_iStreamIndex = av_find_best_stream(a_oFContext, AVMEDIA_TYPE_VIDEO, -1, -1, &a_oCodec, 0) < 0) {
		a_mLogManager->Warning(0, "[RMovie.Fx_LoadMovie] could not find a video stream from url/file \"%s\".", sPath.c_str());
		return;
	}
	a_oCContext = a_oFContext->streams[a_iStreamIndex]->codec;
	av_opt_set_int(a_oCContext, "refcounted_frames", 1, 0);

	if (avcodec_open2(a_oCContext, a_oCodec, NULL) < 0) {
		a_mLogManager->Warning(0, "[RMovie.Fx_LoadMovie] could not open video decoder for movie \"%s\".");
		return;
	}

	a_oDescriptor = new DDMovie(a_oCContext->width, a_oCContext->height, 0, 0, 0, (float)av_q2d(a_oCContext->framerate), 255.0);

	/* PIX_FMT_BGRA PIX_FMT_BGR444 PIX_FMT_RGB444*/
	a_oSContext = sws_getContext(a_oCContext->width, a_oCContext->height, a_oCContext->pix_fmt, a_oCContext->width, a_oCContext->height, PIX_FMT_RGBA, SWS_FAST_BILINEAR, NULL, NULL, NULL);
	
	a_mLogManager->Success(0, "[RMovie.Fx_LoadMovie] \"%s\" loaded.", sPath.c_str());
}

uint8_t* RMovie::Fx_NextFrame(uint8_t* oDestination)
{
	if(a_iFrameFinished) { av_free_packet(&a_sPacket);  }

	while (av_read_frame(a_oFContext, &a_sPacket) >= 0) {
		if (a_sPacket.stream_index == a_iStreamIndex) {
			
			if (avcodec_decode_video2(a_oCContext, a_oFrame, &a_iFrameFinished, &a_sPacket) < 0) {
				a_mLogManager->Warning(0, "[RMovie.Fx_NextFrame] could not decode frame.");
				return NULL;
			}

			if (!a_iFrameFinished) { continue; }

			a_oDescriptor->a_iFrame = a_oCContext->frame_number;
			a_oDescriptor->a_iFrames = a_oDescriptor->a_iFrame;
			a_oDescriptor->a_iPitch = a_oFrame->linesize[0];
			a_oDescriptor->a_iWidth = a_oFrame->width;
			a_oDescriptor->a_iHeight = a_oFrame->height;

			int iPitch = a_oDescriptor->a_iWidth * 4;
			unsigned int iSize = iPitch * a_oDescriptor->a_iHeight * sizeof(float);
			uint8_t* oMemory = (oDestination ? oDestination : (uint8_t*)a_mMemoryManager->Allocate(iSize));

			sws_scale(a_oSContext, (uint8_t const * const *)a_oFrame->data, a_oFrame->linesize, 0, a_oCContext->height, &oMemory, &iPitch);

			return oMemory;
		}

		av_free_packet(&a_sPacket);
	}

	return NULL;
}

uint8_t* RMovie::GetData(uint8_t* oDestination)
{
	return Fx_NextFrame(oDestination);
}

void RMovie::Display(void** oDestination, int iPitch)
{
	if (!oDestination) {
		return;
	}

	sws_scale(a_oSContext, (uint8_t const * const *)a_oFrame->data, a_oFrame->linesize, 0, a_oCContext->height,(uint8_t**)oDestination, &iPitch);
}

IDataDescriptor* RMovie::GetDataDescriptor()
{
	return a_oDescriptor;
}