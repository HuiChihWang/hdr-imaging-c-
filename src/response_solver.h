#pragma once
#include "../include/hdr_imaging_interface.hpp"
#include<Eigen/SparseQR>

class CResponseSolver
{
public:
	CResponseSolver() = default;
	~CResponseSolver() = default;

	void SetImageSequence(const std::vector<TImageExposureTime>& vecImageSequence);
	void SolveResponse();
	cv::Mat GetRadianceMap();

private:
	float GetWeightedCoefficient(float fIntensity);
	void GenerateCoefficientMat();
	bool SolveSparseLinearSystem();
	void GenerateResponse();

	Eigen::SparseMatrix<float> m_spMatCoefficient;
	Eigen::VectorXf m_vecBias;
	Eigen::VectorXf m_vecSolution;

	Eigen::MatrixXf m_matResponseCurve;

	cv::Mat m_matRadiance;

	std::vector<TImageExposureTime> m_vecImageSequence;
	cv::Size m_sizeImage;

	int m_iCurrentChannel = 0;
	const float m_fRegulizer = 0.1f;

	const int m_iZMin = 0;
	const int m_iZmax = 255;
	const int m_iZMid = (m_iZMin + m_iZmax) / 2;
	const int m_iZNumber = m_iZmax - m_iZMin + 1;

};
