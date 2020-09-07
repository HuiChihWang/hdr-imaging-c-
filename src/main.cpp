#include <string>
#include "utility.hpp"
#include "../include/hdr_imaging_interface.hpp"


int main(int argc, char* argv[])
{
	auto vecImageSequence = LoadImageSequence(std::string{ argv[1] });

	auto pHDRImageRecover = CreateHDRRecoverMultiExposure();
	pHDRImageRecover->SetUpImageSequence(vecImageSequence);
	pHDRImageRecover->HDRRecover();
	auto matHDRImage = pHDRImageRecover->GetHDRImage();

	return 0;
}
