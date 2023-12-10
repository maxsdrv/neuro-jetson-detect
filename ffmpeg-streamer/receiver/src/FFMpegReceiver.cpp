#include <iostream>
#include <optional>

#include "FFMpegReceiver.h"

FFMpegReceiver::FFMpegReceiver(std::string sdp_path) :
    _format_context{nullptr},
    _codec_context{nullptr},
    _options{nullptr},
    _sdp_path{std::move(sdp_path)},
    _buffer_h264(buffer_h264_size),
    _avFrame{makeAV<AVFrame>()},
    _packet{makeAV<AVPacket>()} {

    avformat_network_init();
    av_log_set_level(AV_LOG_DEBUG);

    av_dict_set(&_options, "protocol_whitelist", "file,rtp,udp", 0);

    try {
        if (!_avFrame || !_packet) {
            throw std::runtime_error("Failed to allocate AVFrame or AVPacket");
        }

        if (avformat_open_input(&_format_context, _sdp_path.c_str(), nullptr, &_options) < 0) {
            throw std::runtime_error("Failed avformat_open_input.");
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        closeStream();
    }
}

bool FFMpegReceiver::openStream(const std::string& sdp_path) {
    if (avformat_find_stream_info(_format_context, &_options) < 0) {
        std::cerr << "Failed avformat_find_stream_info.\n";
        return false;
    }

    for (size_t i = 0; i < _format_context->nb_streams; ++i) {
        AVStream* stream = _format_context->streams[i];
        AVCodecParameters* codec_parameters = stream->codecpar;
        const AVCodec* decoder = avcodec_find_decoder(codec_parameters->codec_id);
        if (!decoder) {
            std::cerr << "Failed avcodec_find_decoder.\n";
            return false;
        }

        _codec_context = avcodec_alloc_context3(decoder);

        if (avcodec_parameters_to_context(_codec_context, codec_parameters) < 0) {
            std::cerr << "Failed avcodec_parameters_to_context.\n";
            return false;
        }

        if (avcodec_open2(_codec_context, decoder, nullptr) < 0) {
            std::cerr << "Failed avcodec_open2.\n";
            return false;
        }

        return true;
    }

    std::cerr << "No video streams found.\n";

    return false;
}

std::optional<std::vector<unsigned char>> FFMpegReceiver::receiveAndDecodeFrame() {
    _buffer_h264.clear();

    while (av_read_frame(_format_context, _packet.get()) >= 0) {
        if (_packet->stream_index == _format_context->streams[0]->index) {
            if (avcodec_send_packet(_codec_context, _packet.get()) == 0) {
                if (avcodec_receive_frame(_codec_context, _avFrame.get()) == 0) {

                    int Y_plane_size = _avFrame->linesize[0] * _codec_context->height;
                    int U_plane_size = _avFrame->linesize[1] * (_codec_context->height / 2);
                    int V_plane_size = _avFrame->linesize[2] * (_codec_context->height / 2);

                    _buffer_h264.insert(_buffer_h264.end(), _avFrame->data[0], _avFrame->data[0] + Y_plane_size);
                    _buffer_h264.insert(_buffer_h264.end(), _avFrame->data[1], _avFrame->data[1] + U_plane_size);
                    _buffer_h264.insert(_buffer_h264.end(), _avFrame->data[2], _avFrame->data[2] + V_plane_size);

                    av_packet_unref(_packet.get());
                    return _buffer_h264;
                }
            }
            std::cerr << "Failed avcodec_send__packet.\n";
            av_packet_unref(_packet.get());
            break;
        }
        av_packet_unref(_packet.get());
    }

    av_packet_unref(_packet.get());
    return {};
}

FFMpegReceiver::~FFMpegReceiver() {
    closeStream();
}

void FFMpegReceiver::closeStream() {
    if (_format_context) {
        avformat_close_input(&_format_context);
    }
    if (_codec_context) {
        avcodec_free_context(&_codec_context);
    }
    if (_options) {
        av_dict_free(&_options);
    }
}



