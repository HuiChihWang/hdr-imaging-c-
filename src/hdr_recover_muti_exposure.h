#pragma once
#include "../include/hdr_imaging_interface.hpp"

class CHDRRecoverByMutiExposure : public IHDRReconstructor
{
public:
	CHDRRecoverByMutiExposure() = default;

	void SetUpImageSequence(const std::vector<TImageExposureTime>& vecImageSequence) override;
	void HDRRecover() override;
	cv::Mat GetHDRImage() override;

private:

	std::vector<TImageExposureTime> m_vecImageSequence;
	cv::Mat m_matHDRImage;
};
