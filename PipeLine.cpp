#include <vector>
#include <ostream>
#include <fstream>
#include <set>
#include "PipeLine.h"
#include <opencv2/opencv.hpp>
#include "json.hpp"

using json = nlohmann::json;
std::vector<cv::Vec4i> runPhase1_PixelToPhysical();

std::vector<SemanticObject> buildSemanticAnalyzer() {
	
	//get original line segments from phase1, which is the result of HoughLinesP, and the format is [x1, y1, x2, y2]

	std::vector<cv::Vec4i> rawlines = runPhase1_PixelToPhysical();

	//check the box whether to null

	if (rawlines.empty()) {
		return {};
	}

	//use SemanticAnalyzer to analyze the line segments and return vectors of semantic objects -> windows / doors

	SemanticAnalyzer analyzer;
	std::vector<SemanticObject> rawObjects = analyzer.analyze(rawlines);

	//create a null box to store the unique windows, and we can use this box to filter out the duplicate windows that may be caused by the same line segments

	std::vector<SemanticObject> uniqueWindows;

	//create a null box to store the unique doors

	std::vector<SemanticObject> uniqueDoors;
	
	for (const auto& obj : rawObjects) {

		//windows

		if (obj.Type == SemanticType::WINDOW) {

			//if it is true window (Duplicate means a copy object)

			bool isDuplicate = false;

			//saveWin: the unique windows that we have already saved in the box

			for (const auto& saveWin : uniqueWindows) {

				//calculate the distance between two windows, if the distance < 35 pixels, we consider them as duplicate windows

				float dx = obj.pixelPos.x - saveWin.pixelPos.x;
				float dy = obj.pixelPos.y - saveWin.pixelPos.y;

				float centerDist = std::sqrt(dx * dx + dy * dy);

				if (centerDist < 35.0f && std::abs(obj.width - saveWin.width) < 5.0f) {

					isDuplicate = true;
					break;

				}
			}

			//let isDuplicate = false, which means it is a true window, and we can save it in the uniqueWindows box for the next step to filter out the duplicate windows

			if (!isDuplicate) {

				uniqueWindows.push_back(obj);
			}
		
		}

		//doors

		else if (obj.Type == SemanticType::DOOR){

			//if it is true door (Duplicate means a copy object)

			bool isDuplicate = false;

			//saveDoor: the unique doors that we have already saved in the box

			for (const auto& saveDoor : uniqueDoors) {

				//calculate the distance between two doors, if the distance < 25 pixels, we consider them as duplicate doors

				float dx = obj.pixelPos.x - saveDoor.pixelPos.x;
				float dy = obj.pixelPos.y - saveDoor.pixelPos.y;

				float centerDist = std::sqrt(dx * dx + dy * dy);

				if (centerDist < 25.0f) {
					isDuplicate = true;
					break;
				}

			}

			//let isDuplicate = false, which means it is a true window, and we can save it in the uniqueWindows box for the next step to filter out the duplicate windows

			if (!isDuplicate) {

				uniqueDoors.push_back(obj);
			}
		}
	}
	
	//Establish a blacklist for doors and windows

	std::set<size_t> ignoreLineIDs;

	for (const auto& obj : rawObjects) {

		if (obj.Type == SemanticType::WINDOW || obj.Type == SemanticType::DOOR) {
			//if it is a door / window, put it in the ignoreLineIDS boxs
			for (int lineID : obj.sourceLineID) {
				ignoreLineIDs.insert(lineID);
			}


		}
	}

	std::vector<SemanticObject> windows = uniqueWindows;
	std::vector <SemanticObject> doors = uniqueDoors;
 
	std::vector<cv::Vec4i> pureWallLines;
	for (size_t i = 0;i < rawlines.size();i++) {
		if (!ignoreLineIDs.count(i)) {
			pureWallLines.push_back(rawlines[i]);
		}
	}

	std::vector<cv::Vec4i> finalizedWAlls = analyzer.mergeWallSegments(pureWallLines);

	size_t finalTotalCount = finalizedWAlls.size() + windows.size() + doors.size();
	//JSON print data

	json house;
	house["map_info"] = {
	{"name", "Myh_Level_1 -> Level_2"},
	{"count", finalTotalCount},
	{"export_time", "2026-04-15 -> 2026-05-17 -> 2026-05-19 -> 2026-05-24"}
	};

	//WALL

	house["walls"] = json::array();
	int wallCount = 0;

	for (size_t i = 0; i < finalizedWAlls.size(); i++) {
	
		cv::Vec4i l = finalizedWAlls[i];

    	json wall;
 		wall["id"] = wallCount++;
		wall["start"] = { {"x", l[0]}, {"y", l[1]} };
		wall["end"] = { {"x", l[2]}, {"y", l[3]} };
		house["walls"].push_back(wall);
		
	}

	//WINDOW

	house["windows"] = json::array();

	for (size_t i = 0; i < windows.size(); i++) {
		json win;
		win["id"] = i;
		win["center"] = { {"x", windows[i].pixelPos.x}, {"y", windows[i].pixelPos.y} };
		win["width"] = windows[i].width;
		win["rotation"] = windows[i].rotation;
		house["windows"].push_back(win);
	}

	//DOOR

	house["doors"] = json::array();

	for (size_t i = 0; i < doors.size(); i++) {
		json dr;
		dr["id"] = i;
		dr["center"] = { {"x", doors[i].pixelPos.x}, {"y", doors[i].pixelPos.y} };
		dr["width"] = doors[i].width;
		dr["rotation"] = doors[i].rotation;
		house["doors"].push_back(dr);
	}


	std::ofstream out("assets/house.json");
	out << std::setw(4) << house << std::endl;
	out.close();

	return doors;
}