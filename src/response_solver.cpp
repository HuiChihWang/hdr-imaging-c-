#include "response_solver.h"

void CResponseSolver::SetImageSequence(const std::vector<TImageExposureTime>& vecImageSequence)
{
	m_vecImageSequence = vecImageSequence;
}

void CResponseSolver::SolveResponse()
{
	GenerateCoefficientMatrix();
}

cv::Mat CResponseSolver::GetRadianceMap()
{
	return m_matRadiance;
}

float CResponseSolver::GetWeightedCoefficient(float fIntensity)
{
	const float fZmid = static_cast<float>(m_iZMid);
	if (fIntensity < fZmid) {
		return fIntensity - static_cast<float>(m_iZMin);
	}
	else {
		return fIntensity - static_cast<float>(m_iZmax);
	}
}

void CResponseSolver::GenerateCoefficientMatrix()
{
	const int iImageNum = m_vecImageSequence.size();
	const int iPixelNum = m_vecImageSequence[0].matImageFloat.total();

	m_matCoefficientMatrix = cv::Mat::zeros(iImageNum * iPixelNum + m_iZNumber - 1, m_iZNumber + iPixelNum, CV_32FC1);
	m_matBiasMatrix = cv::Mat::zeros(m_matCoefficientMatrix.rows, 1, CV_32FC1);

	int iRowStart = 0;
	for (const auto& tImageSequence : m_vecImageSequence) {
		const cv::Mat matImage = tImageSequence.matImageFloat * 255.f;
		const float fExposureTime = tImageSequence.fExposureTime;

		for (int iRowIdx = 0; iRowIdx < matImage.rows; ++iRowIdx) {
			for (int iColIdx = 0; iColIdx < matImage.cols; ++iColIdx) {

				//TODO: fill in coefficients
				float fPixelIntensity = matImage.at<float>(iRowIdx, iColIdx);
				float fWeight = GetWeightedCoefficient(fPixelIntensity);




				iRowStart += 1;
			}
		}
	}
}

void CResponseSolver::GenerateResponse()
{
	cv::solve(m_matCoefficientMatrix, m_matBiasMatrix, m_matResponseMatrix, cv::DECOMP_SVD);
}
