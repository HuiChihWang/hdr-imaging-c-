#pragma once

#include "opencv2/core.hpp"

struct TImageExposureTime
{
	cv::Mat matImageFloat;
	float fExposureTime;
};


class IHDRReconstructor
{
public:
	virtual ~IHDRReconstructor() = default;
	virtual void SetUpImageSequence(const std::vector<TImageExposureTime>& vecImageSequence) = 0;
	virtual void HDRRecover() = 0;
	virtual cv::Mat GetHDRImage() = 0;
};
