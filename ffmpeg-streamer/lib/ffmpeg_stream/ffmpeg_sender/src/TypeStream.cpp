#include <sstream>

#include "TypeStream.h"

void TypeStream::closeStream() {

}

TypeStream::~TypeStream() {
    closeStream();
    std::cout << __func__ << std::endl;
}

void TypeStream::addStream(AVFormatContext* fmt_ctx, const AVCodecID codec_id) {
    const AVCodec* codec = avcodec_find_encoder(codec_id);

    if (!codec) {
        std::stringstream ss;
        ss << "Could not find encoder for " << avcodec_get_name(codec_id);
        throw std::runtime_error(ss.str());
    }

    _stream = avformat_new_stream(fmt_ctx, nullptr);
    if (!_stream) {
        throw std::runtime_error("Could not make stream.");
    }

    _stream->id = static_cast<int>(fmt_ctx->nb_streams - 1);

    _codec_context = makeAV<AVCodecContext>(codec);
    if (!_codec_context) {
        throw std::runtime_error("Failed avcodec_alloc_context3");
    }

    if (codec->type == AVMEDIA_TYPE_VIDEO) {
        encodeVideo();
    } else if (codec->type == AVMEDIA_TYPE_SUBTITLE){
        encodeMeta();
    }
}

void TypeStream::encodeVideo() {
    _codec_context->bit_rate = 400000;
    _codec_context->time_base = {1, 25};
    _codec_context->gop_size = 25;
    _codec_context->max_b_frames = 1;
    _codec_context->pix_fmt = AVPixelFormat::AV_PIX_FMT_YUV420P;
    _codec_context->codec_type = AVMediaType::AVMEDIA_TYPE_VIDEO;
    _codec_context->flags = AV_CODEC_FLAG_GLOBAL_HEADER;

    av_opt_set(_codec_context->priv_data, "preset", "ultrafast", 0);
    av_opt_set(_codec_context->priv_data, "tune", "zerolatency", 0);
}

void TypeStream::encodeMeta() {

}
