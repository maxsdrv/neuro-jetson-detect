#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>

#include "FFMpegStreamer.h"

FFMpegStreamer::FFMpegStreamer(std::string rtpUrl, const VideoQuality _video_quality) :
    _video_stream{std::make_unique<TypeStream>()},
    _meta_stream{std::make_unique<TypeStream>()},
    _options{nullptr},
    _rtpUrl{std::move(rtpUrl)},
    _frame_count{0}
{
    if (_video_quality == VideoQuality::HIGH) {
        _width = 640;
        _height = 480;
        _frame_rate = 25;
    } else {
        _width = 320;
        _height = 240;
        _frame_rate = 10;
    }
}

FFMpegStreamer::~FFMpegStreamer() {
    std::cout << __func__ << std::endl;
    close();
}

bool FFMpegStreamer::initializeH264() {
    avformat_network_init();
    av_log_set_level(AV_LOG_DEBUG);

    constexpr AVCodecID codec_id = AV_CODEC_ID_H264;
    constexpr AVCodecID meta_codec_id = AV_CODEC_ID_TEXT;

    try {
        _base_fmt_ctx = makeAV<AVFormatContext>();
        _video_stream->addStream(_base_fmt_ctx.get(), codec_id);

    } catch (const std::exception& ex) {
        std::cerr << "Failed initialize video stream " << ex.what() << std::endl;
        close();

        return false;
    }

    if (!initializeEncoder()) {
        std::cerr << "Failed initializeEncoder.\n";
        return false;
    }

    if (!initializeStream()) {
        std::cerr << "Failed initializeStream.\n";
        return false;
    }
    _meta_stream->addStream(_base_fmt_ctx.get(), meta_codec_id);
    /*if (!initializeMetaDataStream(meta_codec_id)) {
        std::cerr << "Failed initializeMetaData.\n";
        return false;
    }*/
    if (!createAVFrame()) {
        std::cerr << "Failed to create AVFrame.\n";
        return false;
    }
    if (!createSdpFile("test.sdp")) {
        std::cerr << "Failed createSdpFile.\n";
        return false;
    }

    return true;
}

bool FFMpegStreamer::sendFrame(const std::vector<unsigned char>& frame) {
    int Y_plane_size = _width * _height;
    int UV_plane_size = Y_plane_size / 4;
    std::copy(frame.begin(), frame.begin() + Y_plane_size, _avFrame->data[0]);
    std::copy(frame.begin() + Y_plane_size, frame.begin() + Y_plane_size + UV_plane_size, _avFrame->data[1]);
    std::copy(frame.begin() + Y_plane_size + UV_plane_size, frame.end(), _avFrame->data[2]);

    _avFrame->pts = _frame_count * av_rescale_q(_frame_count, (AVRational){1, _frame_rate},
                                                _video_stream->getCodecContext()->time_base);
    ++_frame_count;

    if (int ret = avcodec_send_frame(_video_stream->getCodecContext(), _avFrame.get()); ret < 0) {
        std::cerr << "Failed avcodec_send_frame.\n";
        return false;
    }

    int ret = avcodec_receive_packet(_video_stream->getCodecContext(), _video_stream->getPacket());
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        return true;
    } else if (ret < 0) {
        std::cerr << "Failed avcodec_receive_packet.\n";
        return false;
    }

    _video_stream->getPacket()->pts = _video_stream->getPacket()->dts = _avFrame->pts;
    ret = av_interleaved_write_frame(_base_fmt_ctx.get(), _video_stream->getPacket());

    if (ret < 0) {
        std::cerr << "Failed av_interleaved_write_frame.\n";
        return false;
    }

    return true;
}

std::pair<int, int> FFMpegStreamer::frameSize() {
    return {_width, _height};
}

void FFMpegStreamer::close() {
    if (_options) {
        av_dict_free(&_options);
    }
    if (_video_stream) {
        _video_stream->closeStream();
        _meta_stream->closeStream();
    }
}

bool FFMpegStreamer::createSdpFile(const std::string &fileName) {
    std::ostringstream sdpStream;

    AVFormatContext* ac[] = {_base_fmt_ctx.get()};
    char buf[256];
    if (int ret = av_sdp_create(ac, 1, buf, sizeof(buf)); ret < 0) {
        std::cerr << "Failed av_sdp_create.\n";
        return false;
    }
    sdpStream << buf;

    std::ofstream fsdp(fileName);
    if (!fsdp.is_open()) {
        std::cerr << "Could not open SDP file for writing: " << fileName << "\n";
        return false;
    }

    fsdp << sdpStream.str();
    fsdp.close();

    return true;
}

bool FFMpegStreamer::initializeEncoder() {
    _video_stream->getCodecContext()->width = _width;
    _video_stream->getCodecContext()->height = _height;
    _video_stream->getCodecContext()->framerate = {_frame_rate, 1};

    if (avcodec_open2(_video_stream->getCodecContext(), _video_stream->getCodecContext()->codec, nullptr) < 0) {
        std::cerr << "Failed avcodec_open2.\n";
        return false;
    }

    return true;
}

bool FFMpegStreamer::initializeStream() {
    const AVOutputFormat* _format = av_guess_format("rtp", nullptr, nullptr);
    _base_fmt_ctx->oformat = _format;
    _base_fmt_ctx->url = av_strdup(_rtpUrl.c_str());

    av_dict_set(&_options, "protocol_whitelist", "file,rtp,udp", 0);
    av_dict_set(&_options, "sdp_flags", "custom", 0);

    avcodec_parameters_from_context(_video_stream->getStream()->codecpar, _video_stream->getCodecContext());

    if (avio_open(&_base_fmt_ctx->pb, _rtpUrl.c_str(), AVIO_FLAG_WRITE) < 0) {
        std::cerr << "Could not open output URL.\n";
        return false;
    }

    if (avformat_write_header(_base_fmt_ctx.get(), &_options) < 0) {
        std::cerr << "Failed avformat_write_header.\n";
        return false;
    }

    return true;
}

bool FFMpegStreamer::createAVFrame() {
    _avFrame = makeAV<AVFrame>();
    _avFrame->format = _video_stream->getCodecContext()->pix_fmt;
    _avFrame->width = _video_stream->getCodecContext()->width;
    _avFrame->height = _video_stream->getCodecContext()->height;

    if (av_frame_get_buffer(_avFrame.get(), 32) < 0) {
        std::cerr << "Failed av_frame_get_buffer.\n";
        return false;
    }

    if (av_frame_make_writable(_avFrame.get())) {
        std::cerr << "Failed av_frame_make_writable.\n";
        return false;
    }

    return true;
}

bool FFMpegStreamer::initializeMetaDataStream(const AVCodecID codec_id) {

    return true;
}

bool FFMpegStreamer::sendMetaData() {

    return true;
}



