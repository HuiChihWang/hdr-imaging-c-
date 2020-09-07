#include "response_solver.h"

void CResponseSolver::SetImageSequence(const std::vector<TImageExposureTime>& vecImageSequence)
{
	m_vecImageSequence = vecImageSequence;
	m_sizeImage = vecImageSequence[0].matImageFloat.size();
}

void CResponseSolver::SolveResponse()
{
	GenerateCoefficientMat();
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


void CResponseSolver::GenerateCoefficientMat()
{
	const int iImageNum = m_vecImageSequence.size();
	const int iPixelNum = m_sizeImage.area();


	m_spMatCoefficient = Eigen::SparseMatrix<float>(iImageNum * iPixelNum + m_iZNumber - 1, m_iZNumber + iPixelNum);
	m_vecBias = Eigen::VectorXf::Zero(iImageNum * iPixelNum + m_iZNumber - 1);

	int iEquationIdx = 0;
	std::vector<Eigen::Triplet<float>> vecSpCoefficient;
	for (const auto& tImageSequence : m_vecImageSequence) {
		const cv::Mat matImage = tImageSequence.matImageFloat * 255.f;
		const float fExposureTime = tImageSequence.fExposureTime;

		for (int iRowIdx = 0; iRowIdx < matImage.rows; ++iRowIdx) {
			for (int iColIdx = 0; iColIdx < matImage.cols; ++iColIdx) {

				float fPixelIntensity = 255.f * matImage.at<float>(iRowIdx, iColIdx);
				float fWeight = GetWeightedCoefficient(fPixelIntensity);
				int iPixelIndex = matImage.cols * iRowIdx + iColIdx;

				m_vecBias(iEquationIdx) = fWeight * std::logf(fExposureTime);
				vecSpCoefficient.emplace_back(Eigen::Triplet<float>{ iEquationIdx, m_iZNumber + iPixelIndex, -fWeight});
				vecSpCoefficient.emplace_back(Eigen::Triplet<float>{ iEquationIdx, static_cast<int>(fPixelIntensity), fWeight});

				iEquationIdx += 1;
			}
		}
	}

	vecSpCoefficient.emplace_back(Eigen::Triplet<float>{ iEquationIdx, m_iZNumber / 2, 1.f});
	iEquationIdx += 1;

	for (int iZVal = m_iZMin + 1; iZVal < m_iZmax; ++iZVal) {
		float fWeight = GetWeightedCoefficient(static_cast<float>(iZVal));
		int iZIndex = iZVal - m_iZMin;

		vecSpCoefficient.emplace_back(Eigen::Triplet<float>{ iEquationIdx, iZIndex - 1, fWeight* m_fRegulizer});
		vecSpCoefficient.emplace_back(Eigen::Triplet<float>{ iEquationIdx, iZIndex + 1, fWeight* m_fRegulizer});
		vecSpCoefficient.emplace_back(Eigen::Triplet<float>{ iEquationIdx, iZIndex, -2.f * fWeight * m_fRegulizer});

		iEquationIdx += 1;
	}

	m_spMatCoefficient.setFromTriplets(vecSpCoefficient.begin(), vecSpCoefficient.end());
}

void CResponseSolver::SolveSparseLinearSystem()
{
	//TODO: change to SPD solver
	Eigen::LeastSquaresConjugateGradient<Eigen::SparseMatrix<float>> solver;
	solver.compute(m_spMatCoefficient);
	m_vecSolution = solver.solve(m_vecBias);

}

void CResponseSolver::GenerateResponse()
{
	Eigen::SparseSolverBase

		m_matResponseCurve = cv::Mat{ m_iZNumber, 1, CV_32F };

	for (int index = 0; index < m_iZNumber; ++index) {
		m_matResponseCurve.at<float>(index, 0) = matSolution.at<float>(index, 0);
	}

	m_matRadiance = cv::Mat{ m_sizeImage, CV_32F };
	int iPixTotalNum = m_sizeImage.area();
	for (int iPixIndex = 0; iPixIndex < iPixTotalNum; iPixIndex++) {
		int iColIndex = iPixIndex % m_sizeImage.width;
		int iRowIndex = iPixIndex / m_sizeImage.width;

		m_matRadiance.at<float>(iRowIndex, iColIndex) = matSolution.at<float>(iPixIndex + m_iZNumber, 0);
	}
}


