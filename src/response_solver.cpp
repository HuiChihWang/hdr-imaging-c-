#include "response_solver.h"

void CResponseSolver::SetImageSequence(const std::vector<TImageExposureTime>& vecImageSequence)
{
	m_vecImageSequence = vecImageSequence;
	m_sizeImage = vecImageSequence[0].matImageFloat.size();
}

void CResponseSolver::SolveResponse()
{
    m_matRadiance = cv::Mat{m_sizeImage, CV_32FC3};
    m_matResponseCurve = Eigen::MatrixXf(3, m_iZNumber + m_sizeImage.area());

    for (m_iCurrentChannel = 0; m_iCurrentChannel < 3; ++m_iCurrentChannel) {
        GenerateCoefficientMat();
        bool bIsSolve = SolveSparseLinearSystem();

        if (bIsSolve) {
            GenerateResponse();
        }
    }
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
		const cv::Mat matImage = tImageSequence.matImageFloat;
		const float fExposureTime = tImageSequence.fExposureTime;

		for (int iRowIdx = 0; iRowIdx < matImage.rows; ++iRowIdx) {
			for (int iColIdx = 0; iColIdx < matImage.cols; ++iColIdx) {
				float fPixelIntensity = 255.f * matImage.at<cv::Vec3f>(iRowIdx, iColIdx)[m_iCurrentChannel];
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

bool CResponseSolver::SolveSparseLinearSystem()
{
	Eigen::SparseQR<Eigen::SparseMatrix<float>, Eigen::COLAMDOrdering<int>> solver;

	solver.compute(m_spMatCoefficient);
	if (solver.info() != Eigen::Success) {
        return false;
	}

    m_vecSolution = solver.solve(m_vecBias);
    if (solver.info() != Eigen::Success) {
        return false;
    }

	return true;
}

void CResponseSolver::GenerateResponse()
{
    int iPixTotalNum = m_sizeImage.area();

	for (int index = 0; index < m_iZNumber; ++index) {
        m_matResponseCurve(m_iCurrentChannel, index) = m_vecSolution(index);
	}

	for (int iPixIndex = 0; iPixIndex < iPixTotalNum; iPixIndex++) {
		int iColIndex = iPixIndex % m_sizeImage.width;
		int iRowIndex = iPixIndex / m_sizeImage.width;
		m_matRadiance.at<cv::Vec3f>(iRowIndex, iColIndex)[m_iCurrentChannel] = m_vecSolution(iPixIndex + m_iZNumber);
	}
}


