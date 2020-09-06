#include "hdr_recover_muti_exposure.h"

PtrHDRImageRecover CreateHDRRecoverMultiExposure() {
    return std::make_unique<CHDRRecoverByMutiExposure>();
}

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

void CHDRRecoverByMutiExposure::SolveResponse() {
    m_ResponseSolver.SetImageSequence(m_vecImageSequence);
    m_ResponseSolver.SolveResponse();
}
