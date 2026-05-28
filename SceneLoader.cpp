#include <vector>
#include <fstream>
#include "json.hpp"
#include <iostream>
#include <limits>
#include "Mesh.h"
#include "Geometry.h"
#include <glm/glm.hpp>

int runPhase1_PixelToPhysical();
int runPhase2_PixelToPhysical();

using json = nlohmann::json;

Boundingbox cacluclateBounds(const json& data) {

	//NEW: obtain floor data

	float minX = std::numeric_limits<float>::max();
	float minDepth = std::numeric_limits<float>::max(); //JSON -> y
	float maxX = std::numeric_limits<float>::lowest();
	float maxDepth = std::numeric_limits<float>::lowest();

	//find the boundary in thick_walls

	for (const auto& wall : data["thick_walls"]) {
		for (const auto& vertex : wall["vertices_3d"]) {
			float x = vertex["x"];
			float y = vertex["y"];

			if (x < minX)minX = x;
			if (y < minDepth)minDepth = y;
			if (x > maxX)maxX = x;
			if (y > maxDepth)maxDepth = y;

		}
	}

	//leave a 2-meter clearance
	float padding = 0.0f;
	return { minX - padding, minDepth - padding, maxX + padding, maxDepth + padding };
}


void runPhase3_PixelToPhysical(std::vector<Vertex>& outVertices, std::vector<unsigned int>& outIndices) {

	//open processed_house.json file

	std::ifstream f("assets/processed_house.json");

	if (!f.is_open()) {
		std::cerr << "Failed to open the processed_house.json" << std::endl;
		return;
	}

	json data = json::parse(f);

    //use calculateBounds
	
	Boundingbox box = cacluclateBounds(data);

    //convert the json data to float format
	
	// coordinate pool
	// std::vector<float> vertices
	
	// how to get the triangle
	// std::vector<unsigned int> indices

	unsigned int offset = 0; //displacement of  wall

//Lighting model data: add normal vector
//for walls

//important: y-axis is height, x-z is surface
	for (const auto& wall: data["thick_walls"]) {

		//outVertices

		//calculate the center of the wall (8 vertex)
		
		glm::vec3 center(0.0f);

		for (const auto& vertex : wall["vertices_3d"]) {

			//the sum of the 8 vertex's coordinate (x, z, y)
			center += glm::vec3((float)vertex["x"], (float)vertex["z"], (float)vertex["y"]);
		}

		center /= (float)wall["vertices_3d"].size();

		//add vertex + normal vector + uv

		// UV
		int vIdx = 0;

		for (const auto& vertex : wall["vertices_3d"]) {
			
			//(x, y, z) -> (x, z, y)
			float x = (float)vertex["x"];
			float y = (float)vertex["z"];
			float z = (float)vertex["y"];

			//normal vector = vertex - center

			//pos(x, height, depth)

			glm::vec3 pos(x, y, z);
			glm::vec3 normal = glm::normalize(pos - center);

			Vertex v;
			v.Position = pos;
			v.Normal = normal;

			//8 vertex to give uv
			//0,4 -> left-down (0,0) 
			//1,5 -> right-down (1,0)
			//2,6 -> right-up (1,1)
			//3,7 -> left-up (0,1)

			if (vIdx % 4 == 0) v.TexCoords = glm::vec2(0.0f, 0.0f);
			else if (vIdx % 4 == 1) v.TexCoords = glm::vec2(1.0f, 0.0f);
			else if (vIdx % 4 == 2) v.TexCoords = glm::vec2(1.0f, 1.0f);
			else if (vIdx % 4 == 3) v.TexCoords = glm::vec2(0.0f, 1.0f);
			outVertices.push_back(v);

			vIdx++;
		}

		//outIndices

		unsigned int pattern[] = {
			0,1,2,0,2,3, //bottom
			4,5,6,4,6,7, //top
			0,1,5,0,5,4, //front
			3,2,6,3,6,7, //back
			0,3,7,0,7,4, //left
			1,2,6,1,6,5 //right
		};

	    for (unsigned int i: pattern) {
			outIndices.push_back(i + offset);
		}

		//each wall has 8 vertices

		offset += (unsigned int)wall["vertices_3d"].size();
	}

	
//for windows

	for (const auto& window : data["windows"]) {

		//get json window's data
		Window win;
		win.id = (int)window["id"];
		win.center.x = (double)window["center"]["x"];
		win.center.y = (double)window["center"]["y"];
		win.rotation = (double)window["rotation"];
		win.width = (double)window["width"];

		//get 3D coordinate
		WallVertices3D v3d = win.getStretchedVertices(2.8);

		//defining y-axis is height
		glm::vec3 win_points[8] = {
			{v3d.bottom[0].x, 0.0f, v3d.bottom[0].y},
			{v3d.bottom[1].x, 0.0f, v3d.bottom[1].y},
			{v3d.bottom[2].x, 0.0f, v3d.bottom[2].y},
			{v3d.bottom[3].x, 0.0f, v3d.bottom[3].y},
			{v3d.top[0].x, 2.8f, v3d.top[0].y},
			{v3d.top[1].x, 2.8f, v3d.top[1].y},
			{v3d.top[2].x, 2.8f, v3d.top[2].y},
			{v3d.top[3].x, 2.8f, v3d.top[3].y},
		};

		//calculate the center point
		glm::vec3 center(0.0f);

		for (int i = 0;i < 8;i++) {
			center += win_points[i];
		}

		center /= 8.0f;

		//data bag (3 position, 3 normal, 2 texcord)
		for (int i = 0;i < 8;i++) {
			Vertex v;

			//find the point' position in the last screen (the pixel) 
			v.Position = win_points[i];

			//the point's Surface normal direction

			v.Normal = glm::normalize(win_points[i] - center);

			//give uv

			if (i % 4 == 0)v.TexCoords = glm::vec2(-1.0f, 0.0f);
			if (i % 4 == 1)v.TexCoords = glm::vec2(-2.0f, 0.0f);
			if (i % 4 == 2)v.TexCoords = glm::vec2(-2.0f, -1.0f);
			if (i % 4 == 3)v.TexCoords = glm::vec2(-1.0f, -1.0f);

			//put this information into outVertices
			outVertices.push_back(v);
		}
		
		//EBO
		unsigned int pattern[] = {
			//bottom
			0,1,2,0,2,3,
			//top
			4,5,6,4,6,7,
			//left
			0,3,7,0,4,7,
			//right
			1,2,6,1,5,6,
			//forward
			0,1,5,0,4,5,
			//back
			3,2,7,3,6,7
		};

		for (unsigned int i : pattern) {
			outIndices.push_back(i + offset);
		}

		offset += 8;
	}

//for doors

	for (const auto& door : data["doors"]) {

		//get json door's data
		Door dr;
		dr.id = (int)door["id"];
		dr.center.x = (double)door["center"]["x"];
		dr.center.y = (double)door["center"]["y"];
		dr.rotation = (double)door["rotation"];
		dr.width = (double)door["width"];

		//get 3D coordinate
		WallVertices3D v3d = dr.getStretchedVertices(2.8);

		//define the y-axis is height
		glm::vec3 dr_points[8] = {
			{v3d.bottom[0].x, 0.0f, v3d.bottom[0].y},
			{v3d.bottom[1].x, 0.0f, v3d.bottom[1].y},
			{v3d.bottom[2].x, 0.0f, v3d.bottom[2].y},
			{v3d.bottom[3].x, 0.0f, v3d.bottom[3].y},
			{v3d.top[0].x, 2.8f, v3d.top[0].y},
			{v3d.top[1].x, 2.8f, v3d.top[1].y},
			{v3d.top[2].x, 2.8f, v3d.top[2].y},
			{v3d.top[3].x, 2.8f, v3d.top[3].y},
		};

		//calculate the center point
		glm::vec3 center(0.0f);

		for (int i = 0;i < 8;i++) {
			center += dr_points[i];
		}

		center /= 8.0f;

		//data bag (3 position, 3 normal, 2 texcord)
		for (int i = 0;i < 8;i++) {
			Vertex v;

			//find the point' position in the last screen (the pixel) 
			v.Position = dr_points[i];

			//the point's Surface normal direction
			v.Normal = glm::normalize(dr_points[i] - center);

			//give uv

			if (i % 4 == 0)v.TexCoords = glm::vec2(10.0f, 0.0f);
			if (i % 4 == 1)v.TexCoords = glm::vec2(11.0f, 0.0f);
			if (i % 4 == 2)v.TexCoords = glm::vec2(11.0f, 1.0f);
			if (i % 4 == 3)v.TexCoords = glm::vec2(10.0f, 1.0f);

			//put this information into outVertices
			outVertices.push_back(v);
		}

		//EBO
		unsigned int pattern[] = {
			//bottom
			0,1,2,0,2,3,
			//top
			4,5,6,4,6,7,
			//left
			0,3,7,0,4,7,
			//right
			1,2,6,1,5,6,
			//forward
			0,1,5,0,4,5,
			//back
			3,2,7,3,6,7
		};

		for (unsigned int i : pattern) {
			outIndices.push_back(i + offset);
		}

		offset += 8;
	}
	std::cout << "doors count: " << data["doors"].size() << std::endl;

//Lighting model data: add normal vector
//for floor

    //4 vertex (x, y, z) floor

	//add coordinate uv
	auto addFloorVertex = [&](float x, float y, float u, float v) {

		Vertex vert;
		vert.Position = glm::vec3(x, -0.01f, y);
		vert.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
		vert.TexCoords = glm::vec2(u, v);

		outVertices.push_back(vert);

	};

	addFloorVertex(box.minX, box.minDepth, 0.0f, 0.0f); //0,left-down
	addFloorVertex(box.maxX, box.minDepth, 1.0f, 0.0f); //1,right-down
	addFloorVertex(box.maxX, box.maxDepth, 1.0f, 1.0f); //2,right-up
	addFloorVertex(box.minX, box.maxDepth, 0.0f, 1.0f); //3,left-up

	//floor indices

	unsigned int floorIndices[] = { 0, 1, 2, 0, 2, 3 };
	for (unsigned int i : floorIndices) {
		outIndices.push_back(offset + i);
	}
}