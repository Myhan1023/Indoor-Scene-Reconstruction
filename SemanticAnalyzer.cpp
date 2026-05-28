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

				//ID

				winOj.sourceLineID.push_back(i);
				winOj.sourceLineID.push_back(j);

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

		//create a semantic object for the door and add it to the vector
		
		SemanticObject doorOj;

		if (isDoor(lines[i], lines, static_cast<int>(i), doorOj)) {

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

			//ID has been push_back in the function isDoor()

			//all data push_back int the doorOj
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

	float maxAllowedDist = (len1 + len2) /2.0f;

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

bool SemanticAnalyzer::isDoor(const cv::Vec4i& line, const std::vector<cv::Vec4i>& alllines, int lineIdx, SemanticObject& doorOj) {

	//calculate the length of the line segment, which is used for the next step: check whether the line segment is a door frame

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

	//find the minimum distance from the two points to the wall, which is used for the next step: check whether the line segment is a door frame
	
	float p1_min_dist = 9999.0f;
	float p2_min_dist = 9999.0f;

	//make the threshold in 10.0f, means if pointToLineDistance() = distance < 10.0f, it is a door frame

	const float connectThreshold = 5.0f;

	//find which wall can make the line segment a door frame, which means the line segment is close to the wall

	for (const auto& wall : alllines) {

		if (wall == line)continue;

		//skip short wall segments
		float w_dx = static_cast<float>(wall[2] - wall[0]);
		float w_dy = static_cast<float>(wall[3] - wall[1]);

		if (std::sqrt(w_dx * w_dx + w_dy * w_dy) < 80.0f) continue;

		//calculate the distance from p1 and p2 to the wall

		float d1 = pointToLineDistance(p1, wall);
		float d2 = pointToLineDistance(p2, wall);

		//update the minimum distance for p1 and p2
		if (d1 < p1_min_dist) p1_min_dist = d1;
		if (d2 < p2_min_dist) p2_min_dist = d2;

		//check if p1 is close to the wall
		if (!p1_connected && d1 < connectThreshold) {
			p1_connected = true;
		}

		//check if p2 is close to the wall
		if (!p2_connected && d2 < connectThreshold) {
			p2_connected = true;
		}

		//if both points are connected to the wall, it is not a door

		if (p1_connected && p2_connected) {
			return false;
		}
	}
	
	//both points are not connected, it is not a door
	
	if (!p1_connected && !p2_connected) {
		return false; //both points are not connected, it is not a door
	}

	//both points are connected, it is not a door
	
	if (p1_connected && p2_connected) {
		return false;
	}

	//both points are close to the wall, it is not a door

	if (p1_min_dist < 10.0f && p2_min_dist < 10.0f) {
		return false; 
	}

	//one point is connected, one point is not connected, check the distance from the free end to the nearest wall, if it is too far, it is not a door frame

	//freeEnd
	cv::Point2f freeEnd = p1_connected ? p2 : p1;
	
	//hingePoint, means the point that is connected to the wall
	cv::Point2f hingePoint = p1_connected ? p1 : p2;

	float minArcDist = 9999.0f;

	//a temp box to store the line segment IDs that are short lines on the door arc
	std::vector<int> tempArcIds;

	for (size_t k = 0;k < alllines.size();k++) {
		if (k == static_cast<size_t>(lineIdx)) continue;

		//calculate the shortest distance of the freeEnd to the line segment 
		float arcDist = pointToLineDistance(freeEnd, alllines[k]);
		if (arcDist < minArcDist) {
			minArcDist = arcDist;
		}

		//calculate the distance of the arc line segment 
		
		float k_dx = static_cast<float>(alllines[k][2] - alllines[k][0]);
		float k_dy = static_cast<float>(alllines[k][3] - alllines[k][1]);
		float k_len = std::sqrt(k_dx * k_dx + k_dy * k_dy);

		//if the k_len > 50.0f, it is not a short line on the door arc, so we can skip it
		if (k_len > 70.0f)continue;

		//calculate the distance from the hingePoint to the line segment
		float distToHinge = pointToLineDistance(hingePoint, alllines[k]);

		//radius: len, if (distToHinge - radius) < a threshold, it is short lines on the door arc
		if (std::abs(distToHinge - len) < 15.0f) {

			//ignore the short lines on the door arc
			tempArcIds.push_back(static_cast<int>(k)); 
		}

	}

	if (minArcDist > 30.0f) {
		return false;
	}
	
	//put door frame and door Arc
	doorOj.sourceLineID.push_back(lineIdx);
	for (int arcID : tempArcIds) {
		doorOj.sourceLineID.push_back(arcID);
	}

	//one point connected, one point not connected, it is a door frame
	return (p1_connected != p2_connected);

}

//bool the two short line segment whether to merge
bool SemanticAnalyzer::shouldMerge(const cv::Vec4i& line1, const cv::Vec4i& line2, cv::Vec4i& mergedLine) {

	//get the two line segments' startpoints and endpoints
	cv::Point2f p1(static_cast<float>(line1[0]), static_cast<float>(line1[1]));
	cv::Point2f p2(static_cast<float>(line1[2]), static_cast<float>(line1[3]));
	cv::Point2f p3(static_cast<float>(line2[0]), static_cast<float>(line2[1]));
	cv::Point2f p4(static_cast<float>(line2[2]), static_cast<float>(line2[3]));

	//calculate the direction vectors of the two line segments
	cv::Point2f v1 = p2 - p1;
	cv::Point2f v2 = p4 - p3;

	//calculate the lengths of the two line segments 
	float len1 = std::sqrt(v1.x * v1.x + v1.y * v1.y);
	float len2 = std::sqrt(v2.x * v2.x + v2.y * v2.y);

	if (len1 < 1.0f || len2 < 1.0f) return false;

	//1. calculate the angle between the two line segments using the dot product

	float dot = v1.x * v2.x + v1.y * v2.y;
	float costheta = std::abs(dot / (len1 * len2));

	//if the angle is large, we think two line segments are not in the same direction, so we do not merge them

	if (costheta < 0.96f) {
		return false;
	}

	//2. calculate the distance of line1 and line2

	//get the center point of the line1

	cv::Point2f mid_line1 = (p1 + p2) * 0.5f;

	//get Ax+By+C=0 of the line2

	float A = v2.y;
	float B = -v2.x;
	float C = v2.x * p3.y - v2.y * p3.x;

	//if A*A+B*B = line2's length is too small, we don't need to merge 

	if (std::sqrt(v2.y * v2.y + (-v2.x) * (-v2.x)) < 1.0f) return false;

	//get the distance of line1 and line2

	float dist = std::abs(A * mid_line1.x + B * mid_line1.y + C) / std::sqrt(A * A + B * B);

	//if the distance is too large, we think two line segments are not close enough, so we do not merge them

	if (dist > 30.0f) {
		return false;
	}

	//3. calculate the minimum distance of the two line segments

	//list all the points's distance between line1 and line2

	float d13 = static_cast<float>(cv::norm(p1 - p3));
	float d14 = static_cast<float>(cv::norm(p1 - p4));
	float d23 = static_cast<float>(cv::norm(p2 - p3));
	float d24 = static_cast<float>(cv::norm(p2 - p4));

	//find the minimum distance between line1 and line2

	float minGap = std::min({d13, d14, d23, d24});

	//if minGap > 10.0f, we think two line segments are not close enough, so we do not merge them

	if (minGap > 60.0f) {
		return false;
	}

	//if the two line segments pass the above three tests, we can merge them into a new line segment

	std::vector<cv::Point2f> pts = { p1, p2, p3, p4 };
	float maxDist = -1.0f;

	//find the two points that are farthest apart among the four points, we write their ID
    size_t bestI = 0, bestJ = 0;

	for (size_t i = 0;i < pts.size();i++) {
		for (size_t j = i + 1;j < pts.size();j++) {
			float distance = static_cast<float>(cv::norm(pts[i] - pts[j]));
			if (distance > maxDist) {
				maxDist = distance;
				bestI = i;
				bestJ = j;
			}
		}
	}

	mergedLine[0] = static_cast<int>(pts[bestI].x);
	mergedLine[1] = static_cast<int>(pts[bestI].y);
	mergedLine[2] = static_cast<int>(pts[bestJ].x);
	mergedLine[3] = static_cast<int>(pts[bestJ].y);

	return true;
}

//integrate the short lline segment, when they are a long line segment
std::vector<cv::Vec4i> SemanticAnalyzer::mergeWallSegments(const std::vector<cv::Vec4i>& walllines) {

	//put all walllines in a active pool, we can modify them

	std::vector<cv::Vec4i> activePool = walllines;

	//whether we need to scan again
	bool needRescan = true;

	while (needRescan) {

		//if we don't find two line segments need to merge, the while loop directly meets the exit condition
        needRescan = false;

		bool merge = false;

		for (size_t i = 0;i < activePool.size() && !merge;i++) {
			for (size_t j = i + 1;j < activePool.size() && !merge;j++) {

				cv::Vec4i merged;

				if (shouldMerge(activePool[i], activePool[j], merged)) {

					//update activePool[i] = mergedLine , which merge a new line segment
					activePool[i] = merged;
	
					//drop activePool[j], put the vector activePool last point to the activPool[j]'s position
					activePool[j] = activePool.back();

					//cut off the last point's position, which means the activePool length - 1
					activePool.pop_back();

					//because of above operations, the order is a mess, so we need rescan
					needRescan = true;

					//The inner loop j ends
					break;

				}
			}

			//verify the conditions for rescan
			if (needRescan) {

				//the outer loop i ends
				break;
			}
			
		}
	}

	return activePool;
}

