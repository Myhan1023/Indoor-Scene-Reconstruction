#include "Geometry.h"
#include <cmath>

//P(world) = P(pixel) * s(Unit Ratio)

WorldPoint toPhysical(int px_x, int px_y) {
    WorldPoint wp;
    wp.x = px_x * WorldConfig::PIXEL_TO_METER;
    wp.y = px_y * WorldConfig::PIXEL_TO_METER;
    return wp;
}

double Wall::getPhysicalLength() const {
    double dx = end.x - start.x;
    double dy = end.y - start.y;
    return std::sqrt(dx * dx + dy * dy);
}

WallVertices2D Wall::getfoundation() const {
    
    //Calculate the direction vector of the wall

    double dx = end.x - start.x;
    double dy = end.y - start.y;

	//Calculate the length of the wall

    double L_wall = std::sqrt(dx * dx + dy * dy);

    //Calculate the normal vector of the wall

    //Anticlockwise 

    double n_x = -dy;
    double n_y = dx;

    //the unit normal vector

    double nx = n_x / L_wall;
	double ny = n_y / L_wall;

    //dilation of the wall, 4 points

    double half_thickness = thickness / 2.0;
    
    WallVertices2D v2d;

	v2d.p1 = { start.x + half_thickness * nx, start.y + half_thickness * ny };

	v2d.p2 = { start.x - half_thickness * nx, start.y - half_thickness * ny };

	v2d.p3 = { end.x - half_thickness * nx, end.y - half_thickness * ny };

    v2d.p4 = { end.x + half_thickness * nx, end.y + half_thickness * ny };
	
    return v2d;
}

WallVertices3D Wall::getStretchedVertices(double height) const {
    
    WallVertices2D v2d = getfoundation();
    WallVertices3D v3d;

	//define the bottom vertices (z=0)

    v3d.bottom[0] = { v2d.p1.x, v2d.p1.y, 0.0 };
	v3d.bottom[1] = { v2d.p2.x, v2d.p2.y, 0.0 };
	v3d.bottom[2] = { v2d.p3.x, v2d.p3.y, 0.0 };
	v3d.bottom[3] = { v2d.p4.x, v2d.p4.y, 0.0 };

    //define the top vertices (z=height)

    v3d.top[0] = { v2d.p1.x, v2d.p1.y, height };
	v3d.top[1] = { v2d.p2.x, v2d.p2.y, height };
	v3d.top[2] = { v2d.p3.x, v2d.p3.y, height };
    v3d.top[3] = { v2d.p4.x, v2d.p4.y, height };
	
    return v3d;
}

//use windows' information (center,rotation,width) to get windows' coordinate

void Window::getStartEndPoints(WorldPoint& outStart, WorldPoint& outEnd) const {

    //calculate the normlize vector = cos(theta) * L, so L = 1
    double dir_x = std::cos(rotation);
    double dir_y = std::sin(rotation);

    double half_width = width / 2.0f;

    //starting from the center point,walking half_width in the opposite direction get outStart and in the forward direction gets outEnd
    outStart.x = center.x - half_width * dir_x;
    outStart.y = center.y - half_width * dir_y;

    outEnd.x = center.x + half_width * dir_x;
    outEnd.y = center.y + half_width * dir_y;
}

//get foundation of the window, 4 points

WallVertices2D Window::getfoundation() const {

    //get the window's coordinate
    WorldPoint start, end;
    getStartEndPoints(start, end);

    //get the length of the window
    double dx = end.x - start.x;
    double dy = end.y - start.y;
    double L_win = std::sqrt(dx * dx + dy * dy);

    //get vertical normal vector (Anticlockwise: (-dy, dx))
    double nx = -dy / L_win;
    double ny = dx / L_win;

    //dilation of the wall, 4 points

    double half_thickness = thickness / 2.0;

    //get 4 coordinate after dilating
    WallVertices2D v2d;
    v2d.p1 = { start.x + nx * half_thickness, start.y + ny * half_thickness };
    v2d.p2 = { start.x - nx * half_thickness, start.y - ny * half_thickness };
    v2d.p3 = { end.x + nx * half_thickness, end.y + ny * half_thickness };
    v2d.p4 = { end.x - nx * half_thickness, end.y - ny * half_thickness };

    return v2d;

}

//stretching the z-axis of the wall to 3D sapce

WallVertices3D Window::getStretchedVertices(double height) const {

    WallVertices2D v2d = getfoundation();
    WallVertices3D v3d;

    //define the bottom vertices (z=0)
    v3d.bottom[0] = { v2d.p1.x, v2d.p1.y, 0.0 };
    v3d.bottom[1] = { v2d.p2.x, v2d.p2.y, 0.0 };
    v3d.bottom[2] = { v2d.p3.x, v2d.p3.y, 0.0 };
    v3d.bottom[3] = { v2d.p4.x, v2d.p4.y, 0.0 };

    //define the top vertices (z=height)

    v3d.top[0] = { v2d.p1.x, v2d.p1.y, height };
    v3d.top[1] = { v2d.p2.x, v2d.p2.y, height };
    v3d.top[2] = { v2d.p3.x, v2d.p3.y, height };
    v3d.top[3] = { v2d.p4.x, v2d.p4.y, height };

    return v3d;
}

//use doors' information (center,rotation,width) to get doors' coordinate

void Door::getStartEndPoints(WorldPoint& outStart, WorldPoint& outEnd) const {

    //calculate the normlize vector = cos(theta) * L, so L = 1
    double dir_x = std::cos(rotation);
    double dir_y = std::sin(rotation);
    
    double half_width = width / 2.0;

    //starting from the center point,walking half_width in the opposite direction get outStart and in the forward direction gets outEnd
    outStart.x = center.x - dir_x * half_width;
    outStart.y = center.y - dir_y * half_width;
    
    outEnd.x = center.x + dir_x * half_width;
    outEnd.y = center.y + dir_y * half_width;
}

//get foundation of the door, 4 points

WallVertices2D Door::getfoundation() const {

    WorldPoint start, end;
    getStartEndPoints(start, end);

    double dx = end.x - start.x;
    double dy = end.y - start.y;
    double len = sqrt(dx * dx + dy * dy);

    //get vertical normal vector (Anticlockwise: (-dy, dx))
    double nx = -dy / len;
    double ny = dx / len;

    //dilation of the wall, 4 points
    double half_thickness = thickness / 2.0;
    
    //get 4 coordinate after dilating
    WallVertices2D v2d;
    v2d.p1 = { start.x + nx * half_thickness, start.y + ny * half_thickness };
    v2d.p2 = { start.x - nx * half_thickness, start.y - ny * half_thickness };
    v2d.p3 = { end.x + nx * half_thickness, end.y + ny * half_thickness };
    v2d.p4 = { end.x - nx * half_thickness, end.y - ny * half_thickness };

    return v2d;
}

//stretching the z-axis of the door to 3D sapce

WallVertices3D Door::getStretchedVertices(double height) const {

    WallVertices2D v2d = getfoundation();
    WallVertices3D v3d;

    //define the bottom vertices (z=0)
    v3d.bottom[0] = { v2d.p1.x, v2d.p1.y, 0.0 };
    v3d.bottom[1] = { v2d.p2.x, v2d.p2.y, 0.0 };
    v3d.bottom[2] = { v2d.p3.x, v2d.p3.y, 0.0 };
    v3d.bottom[3] = { v2d.p4.x, v2d.p4.y, 0.0 };

    //define the top vertices (z=height)

    v3d.top[0] = { v2d.p1.x, v2d.p1.y, height };
    v3d.top[1] = { v2d.p2.x, v2d.p2.y, height };
    v3d.top[2] = { v2d.p3.x, v2d.p3.y, height };
    v3d.top[3] = { v2d.p4.x, v2d.p4.y, height };

    return v3d;
}