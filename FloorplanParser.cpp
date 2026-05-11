#include "json.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>

using json = nlohmann::json;

int  runPhase1_PixelToPhysical(){

	cv::Mat img = cv::imread("assets/photo.jpg", cv::IMREAD_GRAYSCALE); //变量类型是Mat矩阵
	
	//for picture,check the path
	if (img.empty()) {
		std::cout << "Don't find the image! Please check the path " << std::endl;
		return -1;
	}

	// std::cout << img.cols << std::endl;  362
	// std::cout << img.rows << std::endl;  512
	

	//coordiante(Y,X),Y is the row, X is the column
    
    //Edge detection
	
	cv::Mat edgeImg;

	//Canny
	
	cv::Canny(img, edgeImg, 80, 150);

	//show results of edge detection
	
	cv::imshow("Canny edge", edgeImg);
	cv::waitKey(0);

	//Mrophology, solve the 'hollow double line' issue in canny edge detection
	
	//Define a structuring element (kernel) for dilation, 3*3 tangle
	
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

	//apply dilation
	
	cv::Mat dilatedEdgeImg;

	cv::dilate(edgeImg, dilatedEdgeImg, kernel, cv::Point(-1, -1), 1);

	cv::imshow("dilatedEdege", dilatedEdgeImg);
	cv::waitKey(0);

	//Thinning, solve the 'thick line' issue in dilation

	cv::Mat thinkernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));

	cv::Mat thinnedEdgeImg;
	cv::erode(dilatedEdgeImg, thinnedEdgeImg, thinkernel, cv::Point(-1, -1), 2);

	cv::imshow("thinnedEdge", thinnedEdgeImg);
	cv::waitKey(0);

	//define the vector of line segments [x1, y1, x2, y2]

	std::vector<cv::Vec4i> lines;

	//HoughLinesP
	
	cv::HoughLinesP(thinnedEdgeImg, lines, 1, CV_PI / 180, 20, 10, 30);

	//paint
	
	cv::Mat resultImg;
	cv::cvtColor(img, resultImg, cv::COLOR_GRAY2BGR);

	for (unsigned int i = 0;i < lines.size();i++) {
		cv::Vec4i l = lines[i];

		cv::line(resultImg, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 1);

		std::cout << "line" << i << ": (" << l[0] << "," << l[1] << ") -> (" << l[2] << "," << l[3] << ")" << std::endl;

	}

	std::cout << "Detected lines: " << lines.size() << std::endl;
	cv::imshow("HoughLinesP Result", resultImg);
	cv::waitKey(0);

	//JSON print data
	
	json house;
	house["map_info"] = {
	{"name", "Myh_Level_1"},
	{"count", lines.size()},
	{"export_time", "2026-04-15"} 
	};

	house["walls"] = json::array();

	for (size_t i = 0; i < lines.size(); i++) {
		cv::Vec4i l = lines[i];
		json wall;
		wall["id"] = i;
		wall["start"] = { {"x", l[0]}, {"y", l[1]} };
		wall["end"] = { {"x", l[2]}, {"y", l[3]} };
		house["walls"].push_back(wall);
	}

	std::ofstream out("house.json");
	out << std::setw(4) << house << std::endl;
	out.close();

	//Binariztion
	
	cv::Mat binaryImg;

	// (input image, output image, threshold value, max value, threshold type)
	
	// threshold type: if pixel value > 200, set to 255, otherwise set to 0

	cv::threshold(img, binaryImg, 240, 255, cv::THRESH_BINARY);

	cv::Mat edgeBinaryImg;

	cv::Canny(binaryImg, edgeBinaryImg, 150, 230);

	cv::imshow("Canny edge after binarization", edgeBinaryImg);
	cv::waitKey(0);

    //central computation area (ROI)
	
	int rectWidth = 300; //ROI width
	int rectHeight = 300; //ROI height

	//starting coordinate of the rectangle (top-left corner)
	
	int startX = (img.cols - rectWidth)/2;
	int startY = (img.rows - rectHeight)/2;

	//Define rectangle ROI (x, y, width, height)
	
	cv::Rect roiRect(startX, startY, rectWidth, rectHeight);

	//create ROI
	
	//Don't copy pixel data, just create a new header for the ROI (Shallow Copy)

	cv::Mat roi = binaryImg(roiRect);

	//print ROI
	
	//Iterate through the image (first ten rows of elements)
	for (int i=0;i<roi.rows;i++) {

		//Iterate through the image (first ten colums of elements)
		for (int j = 0;j < roi.cols;j++) {

			int pixelValue = (int)roi.at<uchar>(i, j);

			std::cout << pixelValue << "\t";
		}

		std::cout << std::endl;
	}

	cv::imshow("Original Image", img);
	cv::imshow("Binary Image", binaryImg);
	cv::imshow("ROI Image", roi);
	cv::waitKey(0);
	return 0;
}