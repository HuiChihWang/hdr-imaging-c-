#pragma once
#include <filesystem>
#include <fstream>
#include "opencv2/highgui.hpp"
#include "../include/hdr_imaging_interface.hpp"

namespace fs = std::filesystem;

inline void SplitStringBySpace(const std::string& strSentence, std::vector<std::string>& vecWords)
{
	std::istringstream iss(strSentence);
	vecWords = std::vector<std::string>((std::istream_iterator<std::string>(iss)),
		std::istream_iterator<std::string>());
}

inline std::vector<TImageExposureTime> LoadImageSequence(const std::string& strFolderPath)
{
	std::vector<TImageExposureTime> vecImageSequence;
	for (const auto& itFile : fs::directory_iterator{ strFolderPath })
	{
		if (itFile.path().extension() == ".txt")
		{
			std::ifstream inputFile{ itFile.path() };
			std::string strLine;
			while (std::getline(inputFile, strLine))
			{
				std::vector<std::string> vecFields;
				SplitStringBySpace(strLine, vecFields);

				if (vecFields.size() == 2)
				{
					TImageExposureTime tImageExposure;
					const auto pathFile = fs::path{ strFolderPath } / fs::path{ vecFields[0] };
					const auto matImageUchar = cv::imread(pathFile.string());

					matImageUchar.convertTo(tImageExposure.matImageFloat, CV_32F);
					tImageExposure.matImageFloat /= 255.f;
					tImageExposure.fExposureTime = 1.f / std::stof(vecFields[1]);

					vecImageSequence.emplace_back(tImageExposure);
				}
			}
		}
	}

	return vecImageSequence;
}

