#include "response_solver.h"

void CResponseSolver::SetImageSequence(const std::vector<TImageExposureTime>& vecImageSequence)
{
	m_vecImageSequence = vecImageSequence;
	m_sizeImage = vecImageSequence[0].matImageFloat.size();
}

void CResponseSolver::SolveResponse()
{
	GenerateCoefficientMatrix();
	GenerateResponse();
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

	int iEquationIdx = 0;
	for (const auto& tImageSequence : m_vecImageSequence) {
		const cv::Mat matImage = tImageSequence.matImageFloat * 255.f;
		const float fExposureTime = tImageSequence.fExposureTime;

		for (int iRowIdx = 0; iRowIdx < matImage.rows; ++iRowIdx) {
			for (int iColIdx = 0; iColIdx < matImage.cols; ++iColIdx) {

				float fPixelIntensity = 255.f * matImage.at<float>(iRowIdx, iColIdx);
				float fWeight = GetWeightedCoefficient(fPixelIntensity);
				int iPixelIndex = matImage.cols * iRowIdx + iColIdx;

				m_matBiasMatrix.at<float>(iEquationIdx) = fWeight * std::logf(fExposureTime);
				m_matCoefficientMatrix.at<float>(iEquationIdx, m_iZNumber + iPixelIndex) = -fWeight;
				m_matCoefficientMatrix.at<float>(iEquationIdx, static_cast<int>(fPixelIntensity)) = fWeight;

				iEquationIdx += 1;
			}
		}
	}

	m_matCoefficientMatrix.at<float>(iEquationIdx, m_iZMid) = 0.f;
	iEquationIdx += 1;

	for (int iZVal = m_iZMin + 1; iZVal < m_iZmax; ++iZVal) {
		float fWeight = GetWeightedCoefficient(static_cast<float>(iZVal));

		int iZIndex = iZVal - m_iZMin;
		m_matCoefficientMatrix.at<float>(iEquationIdx, iZIndex - 1) = fWeight * m_fRegulizer;
		m_matCoefficientMatrix.at<float>(iEquationIdx, iZIndex + 1) = fWeight * m_fRegulizer;
		m_matCoefficientMatrix.at<float>(iEquationIdx, iZIndex) = -2.f * fWeight * m_fRegulizer;
		iEquationIdx += 1;
	}
}

void CResponseSolver::GenerateResponse()
{
    cv::Mat matSolution;
	cv::solve(m_matCoefficientMatrix, m_matBiasMatrix, matSolution, cv::DECOMP_SVD);

	m_matResponseCurve = cv::Mat{m_iZNumber, 1, CV_32F};

	for (int index = 0; index < m_iZNumber; ++index) {
	    m_matResponseCurve.at<float>(index, 0) = matSolution.at<float>(index, 0);
	}

	m_matRadiance = cv::Mat{m_sizeImage, CV_32F};
	int iPixTotalNum = m_sizeImage.area();
	for (int iPixIndex = 0; iPixIndex < iPixTotalNum; iPixIndex++) {
	    int iColIndex = iPixIndex % m_sizeImage.width;
	    int iRowIndex = iPixIndex / m_sizeImage.width;

	    m_matRadiance.at<float>(iRowIndex, iColIndex) = matSolution.at<float>(iPixIndex + m_iZNumber, 0);
	}
}


