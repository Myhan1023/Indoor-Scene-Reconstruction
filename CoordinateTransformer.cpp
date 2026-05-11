#include <fstream>
#include "json.hpp"
#include <iostream>
#include "Geometry.h"
#include <iomanip>
#include <vector>
#include "json.hpp"

//Phase 1:pixel_to_physical

int  runPhase1_PixelToPhysical();

using json = nlohmann::json;

int runPhase2_PixelToPhysical() {

    //open house.json file

    std::ifstream f("assets/house.json");

    if (!f.is_open()) {
		std::cerr << "Failed to open house.json file!" << std::endl;
    }

    json data = json::parse(f);
    
    //save house.json data to a vector of walls
    
	std::vector<Wall> allWalls; 

    //solve the JSON file

    for (auto& item : data["walls"]) {

        Wall w;
        w.id = item["id"];
		w.start = toPhysical(item["start"]["x"], item["start"]["y"]);
        w.end = toPhysical(item["end"]["x"], item["end"]["y"]);

    /*    
        std::cout << "Wall" << w.id << "Length:" << w.getPhysicalLength() << "meter" << std::endl;

        WallVertices2D v = w.getfoundation();
		std::cout << "Wall" << w.id << "Foundation:" << std::endl;
        std::cout << "p1: (" << v.p1.x << ", " << v.p1.y << ")" << std::endl;
        std::cout << "p2: (" << v.p2.x << ", " << v.p2.y << ")" << std::endl;
        std::cout << "p3: (" << v.p3.x << ", " << v.p3.y << ")" << std::endl;
		std::cout << "p4: (" << v.p4.x << ", " << v.p4.y << ")" << std::endl;
    */
        //send to next processed_house.json file
        
        allWalls.push_back(w);
    }


    //send the wall information to processed_house.json file

	nlohmann::json processed_houseData;

    processed_houseData["project_info"] = {
           {"name", "60sqm_Apartment_3D_Reconstruction"},
           {"wall_count", allWalls.size()},
           {"unit","meter"},
           { "thickness_unit",0.24 },
           {"default_height",2.8},
           { "export_time", "2026-04-18" }
    };

    processed_houseData["thick_walls"] = json::array();

    for (const auto& w : allWalls) {

        WallVertices3D v3d = w.getStretchedVertices(2.8);

        json wallData;
        wallData["id"] = w.id;
        wallData["length"] = w.getPhysicalLength();

        //save 8 points of the wall in 3D space to Processed_house.json file

        wallData["vertices_3d"] = {
        {{"x",v3d.bottom[0].x},{"y",v3d.bottom[0].y},{"z",v3d.bottom[0].z}},
        {{"x",v3d.bottom[1].x},{"y",v3d.bottom[1].y},{"z",v3d.bottom[1].z}},
        {{"x",v3d.bottom[2].x},{"y",v3d.bottom[2].y},{"z",v3d.bottom[2].z}},
        {{"x",v3d.bottom[3].x},{"y",v3d.bottom[3].y},{"z",v3d.bottom[3].z}},
        {{"x",v3d.top[0].x},{"y",v3d.top[0].y},{"z",v3d.top[0].z}},
        {{"x",v3d.top[1].x},{"y",v3d.top[1].y},{"z",v3d.top[1].z}},
        {{"x",v3d.top[2].x},{"y",v3d.top[2].y},{"z",v3d.top[2].z}},
	    {{"x",v3d.top[3].x},{"y",v3d.top[3].y},{"z",v3d.top[3].z}}
        };

		processed_houseData["thick_walls"].push_back(wallData);
    }

    std::ofstream out("processed_house.json");
    out << std::setw(4) << processed_houseData << std::endl;
    out.close();

    return 0;

}