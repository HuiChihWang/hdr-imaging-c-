#include "hdr_recover_muti_exposure.h"

void CHDRRecoverByMutiExposure::SetUpImageSequence(const std::vector<TImageExposureTime>& vecImageSequence)
{
	m_vecImageSequence = vecImageSequence;
}

void CHDRRecoverByMutiExposure::HDRRecover()
{
}

cv::Mat CHDRRecoverByMutiExposure::GetHDRImage()
{
	return m_matHDRImage;
}
