#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include <glm/glm.hpp>
#include <vector>
#include <opencv2/opencv.hpp>

//define every object type that we want to analyze in the future, and we can add more types if needed

enum class SemanticType {
	WALL,
	WINDOW,
	DOOR,
	OPENING, //no door / window,just null
	NONE //other objects
};

//the struct to store the semantic information of an object, including its type, position, size and rotation angle

struct SemanticObject {
	SemanticType Type;
	glm::vec2 pixelPos; //the image center point in 2D (OpenCV coordinate)
	glm::vec3 worldPos; //transport center point in 3D (OpenGL coordinate)
	float width; //the object's real width
	float rotation; //The deflection angle of an object relative to the coordinate axis
};

//
class SemanticAnalyzer {
public:
	//use HoughlinesP to analyze the line segments and return a vector of semantic objects, which can be used for further processing and visualization
	std::vector<SemanticObject> analyze(const std::vector<cv::Vec4i>& lines);

private:
	//whether the line segment is a window
	bool isWindow(const cv::Vec4i& line1, const cv::Vec4i& line2);

	//whether the lines segment is a door
	bool isDoor(const cv::Vec4i& line, const std::vector<cv::Vec4i>& alllines);

	//two Parament(a point on the line, a wall or a window)
	float pointToLineDistance(const cv::Point2f& p, const cv::Vec4i& line);

};

#endif // !SEMANTIC_ANALYZER_H


