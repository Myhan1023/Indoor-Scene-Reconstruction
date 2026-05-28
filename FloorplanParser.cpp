#include <opencv2/opencv.hpp>
#include <iomanip>
#include <vector>


std::vector<cv::Vec4i> runPhase1_PixelToPhysical(){

	cv::Mat img = cv::imread("assets/photo_2.jpg", cv::IMREAD_GRAYSCALE); 
	
	//for picture,check the path
	if (img.empty()) {
		std::cout << "Don't find the image! Please check the path " << std::endl;
		return {};
	}

	//print image size
	float targetWidth = 1000.0f;

	//resize the image to a fixed width, and keep the aspect ratio unchanged
	float scale = targetWidth / img.cols;
	
	//refese scaling the image
	int targetHeight = static_cast<int>(img.rows * scale);
	
	//resize the image to the target size, using INTER_AREA interpolation method, which is good for shrinking the image
	//six Parameters (input image, output image, dsize, fx, fy, interpolation method), if dsize is specified, fx and fy are ignored (can be 0)
	cv::resize(img, img, cv::Size(targetWidth, targetHeight), 0, 0, cv::INTER_AREA);

	// std::cout << img.cols << std::endl;  362
	// std::cout << img.rows << std::endl;  512
	

	//coordiante(Y,X),Y is the row, X is the column
    
    //Edge detection
	
	cv::Mat edgeImg;

//Canny: find the place which has a large gradient change, and mark it as an edge. The parameters are: (input image, output image, low threshold, high threshold)
	
	cv::Canny(img, edgeImg, 80, 150);

	//show results of edge detection

/*
	cv::imshow("Canny edge", edgeImg);
	cv::waitKey(0);
*/

	//Mrophology, solve the 'hollow double line' issue in canny edge detection
	
	//Define a structuring element (kernel) for dilation, 3*3 tangle
	
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

	//apply dilation

	cv::Mat dilatedEdgeImg;

//Dilation:make two lines close to each other become one line, the parameters are: (input image, output image, kernel, anchor point, iterations)

	cv::dilate(edgeImg, dilatedEdgeImg, kernel, cv::Point(-1, -1), 1);

/*
	cv::imshow("dilatedEdege", dilatedEdgeImg);
	cv::waitKey(0);
*/

    //Thinning, solve the 'thick line' issue in dilation

	cv::Mat thinkernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));

	cv::Mat thinnedEdgeImg;

//Erosion: make one big line to be a thinned line, the parameters are: (input image, output image, kernel, anchor point, iterations)

	cv::erode(dilatedEdgeImg, thinnedEdgeImg, thinkernel, cv::Point(-1, -1), 2);

/*
	cv::imshow("thinnedEdge", thinnedEdgeImg);
	cv::waitKey(0);
*/

	//define the vector of line segments [x1, y1, x2, y2]

	std::vector<cv::Vec4i> lines;

//HoughLinesP: find line segments in the image, the parameters are: (input image, output vector of lines, rho, theta, threshold, minimum line length, maximum line gap)
	
	cv::HoughLinesP(thinnedEdgeImg, lines, 1, CV_PI / 180, 30, 20, 35);

	//paint
	
	cv::Mat resultImg;
	cv::cvtColor(img, resultImg, cv::COLOR_GRAY2BGR);

	for (unsigned int i = 0;i < lines.size();i++) {
		cv::Vec4i l = lines[i];

		cv::line(resultImg, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 1);

		std::cout << "line" << i << ": (" << l[0] << "," << l[1] << ") -> (" << l[2] << "," << l[3] << ")" << std::endl;

	}

	std::cout << "Detected lines: " << lines.size() << std::endl;

/*
	cv::imshow("HoughLinesP Result", resultImg);
	cv::waitKey(0);
*/

/*
	have no value for this step, but I want to show the process of image processing, so I put it here

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

*/

	return lines;
}