#pragma once
#include "../include/hdr_imaging_interface.hpp"

class CResponseSolver
{
public:
	CResponseSolver();
	~CResponseSolver();

	void SetImageSequence(const std::vector<TImageExposureTime>& vecImageSequence);
	void SolveResponse();
	cv::Mat GetRadianceMap();

private:
	float GetWeightedCoefficient(float fIntensity);
	void GenerateCoefficientMatrix();
	void GenerateResponse();

	cv::Mat m_matCoefficientMatrix;
	cv::Mat m_matBiasMatrix;
	cv::Mat m_matResponseMatrix;

	cv::Mat m_matRadiance;
	std::vector<TImageExposureTime> m_vecImageSequence;

	const int m_iZMin = 0;
	const int m_iZmax = 255;
	const int m_iZMid = (m_iZMin + m_iZmax) / 2;
	const int m_iZNumber = m_iZmax - m_iZMin + 1;

};
