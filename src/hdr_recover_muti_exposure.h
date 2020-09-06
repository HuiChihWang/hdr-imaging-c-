#pragma once
#include "../include/hdr_imaging_interface.hpp"
#include "response_solver.h"

class CHDRRecoverByMutiExposure : public IHDRReconstructor
{
public:
	CHDRRecoverByMutiExposure() = default;

	void SetUpImageSequence(const std::vector<TImageExposureTime>& vecImageSequence) override;
	void HDRRecover() override;
	cv::Mat GetHDRImage() override;

private:
    void SolveResponse();

	std::vector<TImageExposureTime> m_vecImageSequence;
	cv::Mat m_matHDRImage;
	CResponseSolver m_ResponseSolver;
};
