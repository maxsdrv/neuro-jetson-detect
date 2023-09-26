#include <iostream>

#include "rtp_server.h"
#include <opencv2/core/cuda.hpp>
#include <cuda.h>
#include <cuda_runtime.h>


int main(int argc, char* argv[]) {
	cuInit(0); // Initialize CUDA

	std::string pipeline{ "rtp://192.168.191.97:5000" };

	videoOptions custom_input_options;
	videoOptions custom_output_options;

	//custom_input_options.resource = "csi://0";
	custom_input_options.resource.protocol = "csi";
	custom_input_options.resource.location = "0";
	custom_input_options.deviceType = videoOptions::DeviceType::DEVICE_CSI;
	custom_input_options.ioType = videoOptions::IoType::INPUT;
	custom_input_options.codec = videoOptions::Codec::CODEC_H264;
	custom_input_options.width = 1280;
	custom_input_options.height = 720;
	custom_input_options.frameRate = 30;
	custom_input_options.numBuffers = 4;
	custom_input_options.zeroCopy = true;
	custom_input_options.flipMethod = videoOptions::FlipMethod::FLIP_HORIZONTAL;

	videoSource* input = videoSource::Create(custom_input_options);

	if (!input) {
		std::cerr << "Failed create input.\n";
		return -1;
	}

	custom_output_options.resource.protocol = "rtp";
	custom_output_options.resource.location = "192.168.191.97";
	custom_output_options.resource.port = 5000;
	custom_output_options.deviceType = videoOptions::DeviceType::DEVICE_IP;
	custom_output_options.ioType = videoOptions::IoType::OUTPUT;
	custom_output_options.codec = videoOptions::Codec::CODEC_H264;
	custom_output_options.codecType = videoOptions::CodecType::CODEC_OMX;
	custom_output_options.frameRate = 30;
	custom_output_options.bitRate = 4000000;
	custom_output_options.numBuffers = 4;
	custom_output_options.zeroCopy = true;
	custom_output_options.latency = 10;

	videoOutput* output = videoOutput::Create(custom_output_options);

	if (!output) {
		std::cerr << "Failed create output.\n";
		return -1;
	}

	while (true) {
		uchar3* image = nullptr;
		int status = 0;

		if (!input->Capture(&image, 1000, &status)) {
			if (status == videoSource::TIMEOUT) {
				continue;
			}
			break;
		}

		cv::cuda::GpuMat dummy_frame(input->GetHeight(), input->GetWidth(), CV_8UC3);

		if (output != nullptr) {
			output->Render(image, input->GetWidth(), input->GetHeight());

			if (!output->IsStreaming()) {
				break;
			}
		}
	}

	SAFE_DELETE(input);
	SAFE_DELETE(output);
}