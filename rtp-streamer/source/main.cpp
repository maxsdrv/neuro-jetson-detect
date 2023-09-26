#include <iostream>
#include <jetson-utils/videoSource.h>
#include <jetson-utils/videoOutput.h>


int main(int argc, char* argv[]) {
	std::string pipeline{ "rtp://192.168.191.97:5000" };

	videoOptions customOptions;

	customOptions.resource = "csi://0";
	customOptions.deviceType = videoOptions::DeviceType::DEVICE_CSI;
	customOptions.ioType = videoOptions::IoType::INPUT;
	customOptions.codec = videoOptions::Codec::CODEC_H264;
	customOptions.width = 1280;
	customOptions.height = 720;
	customOptions.frameRate = 30;
	customOptions.numBuffers = 4;
	customOptions.zeroCopy = true;
	customOptions.flipMethod = videoOptions::FlipMethod::FLIP_HORIZONTAL;

	videoSource* input = videoSource::Create(customOptions);
	videoOutput* output = videoOutput::Create(pipeline.c_str());

	if (!input) {
		std::cerr << "Failed create input.\n";
		return -1;
	}

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