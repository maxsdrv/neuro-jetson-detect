#ifndef FMPEG_STREAMER_H_
#define FMPEG_STREAMER_H_

#include <string>
#include <vector>

#include "TypeStream.h"

enum class VideoQuality {
    LOW,
    HIGH
};


/**
 * FFMpegStreamer is a class that is responsible for streaming video frames using FFmpeg.
 */
class FFMpegStreamer {
public:
    explicit FFMpegStreamer(std::string rtpUrl, VideoQuality videoQuality = VideoQuality::HIGH);
    ~FFMpegStreamer();

    /**
     * Initialize the streamer with the given RTP URL.
     *
     * @param rtpUrl The RTP URL to stream the video frames to.
     * @return true if the initialization was successful, false otherwise.
     */
    [[nodiscard]] bool initializeH264();

    /**
     * Encode the given frame and stream it.
     *
     * @param frame The raw video frame to encode and stream.
     * @return true if the encoding and streaming was successful, false otherwise.
     */
    [[nodiscard]] bool sendFrame(const std::vector<unsigned char>& frame);

    /**
     * Get the width and height of the frame.
     *
     * @return A pair of integers where the first element is the width and the second element is the height of the frame.
     */
    std::pair<int, int> frameSize();

    /**
     * Close the stream.
     */
    void close();

private:
    std::unique_ptr<AVFrame, std::function<void(AVFrame *)>> _avFrame;
    std::unique_ptr<AVFormatContext, std::function<void(AVFormatContext *)>> _base_fmt_ctx;
    std::unique_ptr<TypeStream> _video_stream;
    std::unique_ptr<TypeStream> _meta_stream;
    AVDictionary *_options;

    std::string _rtpUrl;
    int _width;
    int _height;
    int64_t _frame_count;
    int _frame_rate;
    /**
     * Utility function to create and manage FFmpeg resources.
     *
     * @param <T> The type of the FFmpeg resource.
     * @param <D> The deleter type for the FFmpeg resource.
     * @return A unique pointer to the created resource.
     */
    template<typename T, typename... Args>
    std::unique_ptr<T, std::function<void(T *)>> makeAV(Args &&... args) {
        if constexpr (std::is_same_v<T, AVFrame>) {
            T *ptr = av_frame_alloc();
            if (!ptr) {
                throw std::runtime_error("Failed to allocate AVFrame");
            }
            return std::unique_ptr<T, std::function<void(T *)>>(
                    ptr, [](T *ptr) {
                        av_frame_free(&ptr);
                    });
        }
        if constexpr (std::is_same_v<T, AVFormatContext>) {
            T* ptr = avformat_alloc_context();
            if (!ptr) {
                throw std::runtime_error("Failed to allocate AVFormatContext");
            }
            return std::unique_ptr<T, std::function<void(T *)>>(
                    ptr, [](T *ptr) {
                        avformat_free_context(ptr);
                    });
        }
    }

    /**
     *
     * @param codec_id  Desired codec id
     * @return flag if initialization was successful
     */
    [[nodiscard]] bool initializeEncoder();
    /**
     *  Initialize stream
     * @return flag if initialization was successful
     */
    [[nodiscard]] bool initializeStream();
    /**
     * Create AVFrame
     * @return  flag if initialization was successful
     */
    [[nodiscard]] bool createAVFrame();

    /**
     * Create sdp file for VLC
    */
    bool createSdpFile(const std::string& fileName);

    /**
     * Initialize metadata
     * @return true if initialization was successful
     */
    bool initializeMetaDataStream(AVCodecID codec_id);

    /**
     * Send metadata
     * @return true if sending was successful
     */
    bool sendMetaData();

};
#endif
