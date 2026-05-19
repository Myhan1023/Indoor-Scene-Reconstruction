#include "SemanticAnalyzer.h"
#include <cmath>
#include <iostream>

std::vector<SemanticObject> SemanticAnalyzer::analyze(const std::vector<cv::Vec4i>& lines) {
	
	//create a null box
	std::vector<SemanticObject> detectedObjects;

	//initialize all lines = false, means all lines are not used for windows, doors or walls
	std::vector<bool> usedLines(lines.size(), false);

//Windows detection
	//Pair each line segment with each of the subsequent line segments in pairs.
	for (size_t i = 0; i < lines.size();i++) {
		for (size_t j = i + 1;j < lines.size();j++) {

			//find true window and write the semantic information into the winOj ,finally pass to detectedObjects vector (Type, Pixel, width, rotation)

			if (isWindow(lines[i], lines[j])) {
				
				//create a semantic object for the window and add it to the vector
				SemanticObject winOj;

				winOj.Type = SemanticType::WINDOW;

				//find the center point of two segment lines
				winOj.pixelPos.x = (float)((lines[i][0] + lines[i][2] + lines[j][0] + lines[j][2]) / 4.0f);

				winOj.pixelPos.y = (float)((lines[i][1] + lines[i][3] + lines[j][1] + lines[j][3]) / 4.0f);

				//calculate the average length of the two lines as the window's width

				//direction vector of line
				float diffx1 = (float)(lines[i][2] - lines[i][0]);
				float diffy1 = (float)(lines[i][3] - lines[i][1]);
				float len1 = (float)(sqrt(diffx1 * diffx1 + diffy1 * diffy1));

				float diffx2 = (float)(lines[j][2] - lines[j][0]);
				float diffy2 = (float)(lines[j][3] - lines[j][1]);
				float len2 = (float)(sqrt(diffx2 * diffx2 + diffy2 * diffy2));	

			//	winOj.width = (len1 + len2) / 2.0f;

			
				//which is short, which is a window
				
				if (len1 < len2) {
					winOj.width = len1;

					//find the center point of segment line
					winOj.pixelPos.x = (float)(lines[i][2] + lines[i][0]) / 2.0f;
					winOj.pixelPos.y = (float)(lines[i][3] + lines[i][1]) / 2.0f;

				}
				else {
					winOj.width = len2;

					//find the center point of segment line
					winOj.pixelPos.x = (float)(lines[j][2] + lines[j][0]) / 2.0f;
					winOj.pixelPos.y = (float)(lines[j][3] + lines[j][1]) / 2.0f;

				}

                //trop width = 272 , which is in right-down
				
				if (winOj.pixelPos.x > 900.0f &&
					winOj.pixelPos.y > 1100.0f &&
					winOj.width > 270.0f && winOj.width < 275.0f) {
					continue; 
				}

				//rotation

				winOj.rotation = std::atan2(diffy1, diffx1);

				detectedObjects.push_back(winOj);

				//mark the two lines as used for window detection

				usedLines[i] = true;
				usedLines[j] = true;
			}
		}
	}

//door detection

	//Topological method

	for (size_t i = 0;i < lines.size();i++) {

		//avoid this line segment has been used for window detection
		if (usedLines[i])continue;

		if (isDoor(lines[i], lines)) {

			//create a semantic object for the door and add it to the vector
			SemanticObject doorOj;

			doorOj.Type = SemanticType::DOOR;

			//find the center point of the line segment as the door's pixel position

			doorOj.pixelPos.x = (float)(lines[i][0] + lines[i][2]) / 2.0f;
			doorOj.pixelPos.y = (float)(lines[i][1] + lines[i][3]) / 2.0f;

			//caculate the width of the door, which is the length of the line segment

			float diffx = (float)(lines[i][2] - lines[i][0]);
			float diffy = (float)(lines[i][3] - lines[i][1]);
			doorOj.width = std::sqrt(diffx * diffx + diffy * diffy);

            //caculate roration

			doorOj.rotation = std::atan2(diffy, diffx);

			detectedObjects.push_back(doorOj);

			//mark the line as used for door detection

			usedLines[i] = true;
		}
	}
	return detectedObjects;
}

//Determine whether it is a window.

bool SemanticAnalyzer::isWindow(const cv::Vec4i& line1, const cv::Vec4i& line2) {

	//Parallelism check

	//Calculate the angle between the two lines by DOT

	//direction vector of line1

	float diffx1 = (float)(line1[2] - line1[0]);
	float diffy1 = (float)(line1[3] - line1[1]);

	//direction vector of line2

	float diffx2 = (float)(line2[2] - line2[0]);
	float diffy2 = (float)(line2[3] - line2[1]);

	// |a|, is length of line1

	float len1 = sqrt(diffx1 * diffx1 + diffy1 * diffy1);

	// |b|, is length of line2

	float len2 = sqrt(diffx2 * diffx2 + diffy2 * diffy2);

	//Exclude pixel line segments that are too short.
	// Avoid division by zero (if len1 * len2 approaches 0) 
	// and filter out short, noisy line segments whose direction vectors 
	// are unstable and highly susceptible to pixel quantization errors.

	if (len1 < 5.0f || len2 < 5.0f) return false;

	//refuse the lines that have a large length difference, which is not likely to be a window frame
	
	//if (std::abs(len1 - len2) > 30.0f)return false;

	//dot : a * b = |a| * |b| * cos(theta)
	// a (diffx1, diffy1) , b (diffx2, diffy2)

	float dot = diffx1 * diffx2 + diffy1 * diffy2;

	float costheta = std::abs(dot / (len1 * len2));

	//Non-parallel

	if (costheta < 0.99f) return false;

	//Distance check

	//due to lines has been parallel, so we only make a point in line1 and its distance to line2 

	//find the point in line1

	float x0 = (float)( (line1[0] + line1[2]) / 2.0f );
	float y0 = (float)( (line1[1] + line1[3]) / 2.0f );

	//calculate the line2's Linear equation: Ax+By+C=0

	float A = diffy2;
	float B = -diffx2;
	float C = (float)(diffx2 * line2[1] - diffy2 * line2[0]);

	//calculate the distance from the point to the line2

	float dist = std::abs(A * x0 + B * y0 + C) / std::sqrt(A * A + B * B);

	//check the distance between the two lines in [5, 40] pixels, which is a reasonable range for window frames in typical indoor scenes

	if (dist < 5.0f || dist > 20.0f) return false;

	//Overlap check

	//calculate two lines' center points, for the next step: calculate two lines' real distance

	float midX2 = (float)( (line2[2] + line2[0]) / 2.0f );
	float midY2 = (float)( (line2[3] + line2[1]) / 2.0f );

	//for the next step: calculate two lines' real physical distance (Pythagorean theorem)

	float midDist = std::sqrt((midX2 - x0) * (midX2 - x0) + (midY2 - y0) * (midY2 - y0));

	//The maximum allowed midpoint distance is the length of the longest line among the two line segments

	float maxAllowedDist = len1 + len2 /2.0f;

	//if two center points'distance is too far than maximum (len1,len2), so it is not a window

	if (midDist > maxAllowedDist) return false;

	//Exclude pixel line segments that are between [350.0f, 400.0f]
	
	if (len1 < 60.0f || len2 < 60.0f) return false;

	//all check are pass, it is a window
	return true;
}

//find the shortest distance from a point to a line segment, which is used for door detection

float SemanticAnalyzer::pointToLineDistance(const cv::Point2f& p, const cv::Vec4i& line) {

	//get the line's startpoint A and endpoint B

	cv::Point2f a(static_cast<float>(line[0]), static_cast<float>(line[1]));
	cv::Point2f b(static_cast<float>(line[2]), static_cast<float>(line[3]));

	//direction vector of the line segment AB and AP

	cv::Point2f ab = b - a;
	cv::Point2f ap = p - a;

	//the length of AB

	float abLen = std::sqrt(ab.x * ab.x + ab.y * ab.y);

	//avoid division by zero

	if (abLen == 0) {
		return static_cast<float>(cv::norm(p - a));
	}

	//calculate t,which is the projection of AP onto AB, get AC on the AB, then t = AC / AB

	float t = (ap.x * ab.x + ap.y * ab.y) / (abLen * abLen);

	//if t is less than 0, the closest point is A; if t is greater than 1, the closest point is B; otherwise, the closest point is on the line segment AB

	if (t < 0.0f) t = 0.0f; //if run away from A, return A
	if (t > 1.0f) t = 1.0f; //if run away from B, return B

	//calculate the closest point on the line segment AB to the point P

	cv::Point2f closest = a + t * ab;

	//return the distance from the point P to the closest point on the line segment AB
	//norm() function, caculate the phiscal distance between P and the closest point

	return static_cast<float>(cv::norm(p - closest));
}

//whether the lines segment is a door

bool SemanticAnalyzer::isDoor(const cv::Vec4i& line, const std::vector<cv::Vec4i>& alllines) {

	//caculate the length of the line segment, which is used for the next step: check whether the line segment is a door frame

	float dx = (float)(line[2] - line[0]);
	float dy = (float)(line[3] - line[1]);
	float len = std::sqrt(dx * dx + dy * dy);

	//which line segment is in the reasonable range (100.0f, 150.0f), which can to next step

	if (len < 100.0f || len > 150.0f) return false;

	//get the line's point -> endpoint and startpiont, for the function "pointToLineDistance()"

	cv::Point2f p1(static_cast<float>(line[0]), static_cast<float>(line[1]));
	cv::Point2f p2(static_cast<float>(line[2]), static_cast<float>(line[3]));

	//check which point is near the wall

	bool p1_connected = false;
	bool p2_connected = false;

	//make the threshold in 10.0f, means if pointToLineDistance() = distance < 10.0f, it is a door frame

	const float connectThreshold = 10.0f;

	//find which wall can make the line segment a door frame, which means the line segment is close to the wall

	for (const auto& wall : alllines) {

		if (wall == line)continue;

		//skip short wall segments
		float w_dx = static_cast<float>(wall[2] - wall[0]);
		float w_dy = static_cast<float>(wall[3] - wall[1]);

		if (std::sqrt(w_dx * w_dx + w_dy * w_dy) < 80.0f) continue;

		//check if p1 is close to the wall
		if (!p1_connected && pointToLineDistance(p1, wall) < connectThreshold) {
			p1_connected = true;
		}

		//check if p2 is close to the wall
		if (!p2_connected && pointToLineDistance(p2, wall) < connectThreshold) {
			p2_connected = true;
		}

		//if both points are connected to the wall, it is not a door

		if (p1_connected && p2_connected) {
			return false;
		}
	}
	
	//one point connected, one point not connected, it is a door frame
	return (p1_connected != p2_connected);

}