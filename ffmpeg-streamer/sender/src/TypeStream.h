#ifndef FFMPEG_STREAMER_TYPESTREAM_H
#define FFMPEG_STREAMER_TYPESTREAM_H

#include <iostream>
#include <memory>
#include <functional>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libavutil/frame.h>
#include <libavutil/opt.h>
#include <libavutil/error.h>
#include <libavutil/samplefmt.h>
#include <libavutil/mem.h>
}

class Packet {
public:
    Packet() : _packet{av_packet_alloc()} {}
    ~Packet() {
        av_packet_free(&_packet);
        std::cout << __func__ << std::endl;
    }

    Packet(const Packet&) = delete;
    Packet& operator=(const Packet&) = delete;

    Packet(Packet&& other) noexcept {
        _packet = other._packet;
        other._packet = nullptr;
    }
    Packet& operator=(Packet&& other) noexcept {
        if (this != &other) {
            av_packet_free(&_packet);
            _packet = other._packet;
            other._packet = nullptr;
        }
        return *this;
    }

    [[nodiscard]] AVPacket* getPacket() const {
        return _packet;
    }

private:
    AVPacket* _packet;
};

class TypeStream {
public:
    TypeStream() = default;
    ~TypeStream();

    TypeStream(const TypeStream &) = delete;
    TypeStream& operator=(const TypeStream &) = delete;
    TypeStream(TypeStream &&) = delete;
    TypeStream& operator=(TypeStream &&) = delete;

    struct SwsContext* sws_ctx{nullptr};
    struct SwrContext* swr_ctx{nullptr};

    void addStream(AVFormatContext* fmt_ctx, AVCodecID codec_id);

    [[nodiscard]] AVCodecContext* getCodecContext() const {
        return _codec_context.get();
    }

    [[nodiscard]] AVPacket* getPacket() const {
        return _packet->getPacket();
    }

    [[nodiscard]] AVStream* getStream() const {
        return _stream;
    }

    void closeStream();

private:
    std::unique_ptr<AVCodecContext, std::function<void(AVCodecContext *)>> _codec_context;
    AVStream* _stream{nullptr};
    std::unique_ptr<Packet> _packet{std::make_unique<Packet>()};

    void encodeVideo();
    void encodeMeta();

    /**
     * Utility function to create and manage FFmpeg resources.
     *
     * @param <T> The type of the FFmpeg resource.
     * @param <D> The deleter type for the FFmpeg resource.
     * @return A unique pointer to the created resource.
     */
    template<typename T, typename... Args>
    std::unique_ptr<T, std::function<void(T *)>> makeAV(Args &&... args) {
        if constexpr (std::is_same_v<T, AVCodecContext>) {
            T *ptr = avcodec_alloc_context3(std::forward<Args>(args)...);
            if (!ptr) {
                throw std::runtime_error("Failed to allocate AVCodecContext");
            }
            return std::unique_ptr<T, std::function<void(T *)>>(
                    ptr, [](T *ptr) {
                        avcodec_free_context(&ptr);
                    });
        }
    }
};


#endif //FFMPEG_STREAMER_TYPESTREAM_H
