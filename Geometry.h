#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <glm/glm.hpp>
#include "Constants.h"

struct WorldPoint {

	//meter
	double x;
	double y;

};

//foundation of the wall, 4 points

struct WallVertices2D {
	WorldPoint p1, p2, p3, p4;
};

//3D points

struct WorldPoint3D {

	double x;
	double y;
	double z;

};

//8 points of the wall in 3D space

struct WallVertices3D {

	WorldPoint3D top[4];
	WorldPoint3D bottom[4];

};

//Encapsulate the wall information

struct Wall {

	int id;
	WorldPoint start;
	WorldPoint end;

	//define the wall's thickness

	double thickness = 0.24;

	//get foundation of the wall, 4 points

	WallVertices2D getfoundation() const;
	
	//get the physical length of the wall

	double getPhysicalLength() const;

	//stretching the z-axis of the wall to 3D sapce

	WallVertices3D getStretchedVertices(double height = 2.8) const;

};

//scene boundary

struct Boundingbox {
	float minX, minDepth;
	float maxX, maxDepth;
};

//interface

WorldPoint toPhysical(int px_x, int px_y);


#endif