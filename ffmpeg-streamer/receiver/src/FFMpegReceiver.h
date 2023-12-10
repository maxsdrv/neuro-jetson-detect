#ifndef FMPEG_RECEIVER_H_
#define FMPEG_RECEIVER_H_

#include <string>
#include <vector>
#include <functional>
#include <memory>

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

constexpr int buffer_h264_size = 500000; // 500 KB buffer for H264 encoded frames(460800 bytes for 640x480 frame)

class FFMpegReceiver {
public:
    explicit FFMpegReceiver(std::string sdp_path);
    ~FFMpegReceiver();

    /**
     * Open the stream with the given SDP file.
     * @param _sdp_path is the path to the SDP file.
     * @return true if the stream was opened successfully, false otherwise.
     */
    [[nodiscard]] bool openStream(const std::string& _sdp_path);
    /**
     * Receive and decode a frame from the stream.
     * @return frame if the frame was received and decoded successfully, std::nullopt otherwise.
     */
    std::optional<std::vector<unsigned char>> receiveAndDecodeFrame();

private:
    AVFormatContext* _format_context;
    AVCodecContext* _codec_context ;
    AVDictionary *_options;
    std::string _sdp_path;

    /*
     * Buffer for H264 encoded frames.
     */
    std::vector<unsigned char> _buffer_h264;

    std::unique_ptr<AVFrame, std::function<void(AVFrame *)>> _avFrame;
    std::unique_ptr<AVPacket, std::function<void(AVPacket *)>> _packet;

    /**
     * Close the stream.
     */
    void closeStream();

    /**
     * Create a unique pointer to an AVFrame or AVPacket.
     * @tparam T is the type of the pointer.
     * @return unique pointer to an AVFrame or AVPacket.
     */
    template <typename T>
    std::unique_ptr<T, std::function<void(T *)>> makeAV() {
        if constexpr (std::is_same_v<T, AVFrame>) {
            T *ptr = av_frame_alloc();
            if (!ptr) {
                throw std::runtime_error("Failed to allocate frame.\n");
            }
            return std::unique_ptr<T, std::function<void(T *)>>(
                    ptr, [](T *p) {
                        av_frame_free(&p);
                    });
        }
        else if constexpr (std::is_same_v<T, AVPacket>) {
            T *ptr = av_packet_alloc();
            if (!ptr) {
                throw std::runtime_error("Failed to allocate packet.\n");
            }
            return std::unique_ptr<T, std::function<void(T *)>>(
                    ptr, [](T *p) {
                        av_packet_free(&p);
                    });
        }
    }

};

#endif