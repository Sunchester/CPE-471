/* Release code for program 1 CPE 471 Fall 2016 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "Image.h"

// This allows you to skip the `std::` in front of C++ standard library
// functions. You can also say `using std::cout` to be more selective.
// You should never do this in a header file.
using namespace std;

int g_width, g_height;

/*
   Helper function you will want all quarter
   Given a vector of shapes which has already been read from an obj file
   resize all vertices to the range [-1, 1]
 */
void resize_obj(std::vector<tinyobj::shape_t> &shapes){
   float minX, minY, minZ;
   float maxX, maxY, maxZ;
   float scaleX, scaleY, scaleZ;
   float shiftX, shiftY, shiftZ;
   float epsilon = 0.001;

   minX = minY = minZ = 1.1754E+38F;
   maxX = maxY = maxZ = -1.1754E+38F;

   //Go through all vertices to determine min and max of each dimension
   for (size_t i = 0; i < shapes.size(); i++) {
      for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
         if(shapes[i].mesh.positions[3*v+0] < minX) minX = shapes[i].mesh.positions[3*v+0];
         if(shapes[i].mesh.positions[3*v+0] > maxX) maxX = shapes[i].mesh.positions[3*v+0];

         if(shapes[i].mesh.positions[3*v+1] < minY) minY = shapes[i].mesh.positions[3*v+1];
         if(shapes[i].mesh.positions[3*v+1] > maxY) maxY = shapes[i].mesh.positions[3*v+1];

         if(shapes[i].mesh.positions[3*v+2] < minZ) minZ = shapes[i].mesh.positions[3*v+2];
         if(shapes[i].mesh.positions[3*v+2] > maxZ) maxZ = shapes[i].mesh.positions[3*v+2];
      }
   }

	//From min and max compute necessary scale and shift for each dimension
   float maxExtent, xExtent, yExtent, zExtent;
   xExtent = maxX-minX;
   yExtent = maxY-minY;
   zExtent = maxZ-minZ;
   if (xExtent >= yExtent && xExtent >= zExtent) {
      maxExtent = xExtent;
   }
   if (yExtent >= xExtent && yExtent >= zExtent) {
      maxExtent = yExtent;
   }
   if (zExtent >= xExtent && zExtent >= yExtent) {
      maxExtent = zExtent;
   }
   scaleX = 2.0 /maxExtent;
   shiftX = minX + (xExtent/ 2.0);
   scaleY = 2.0 / maxExtent;
   shiftY = minY + (yExtent / 2.0);
   scaleZ = 2.0/ maxExtent;
   shiftZ = minZ + (zExtent)/2.0;

   //Go through all verticies shift and scale them
   for (size_t i = 0; i < shapes.size(); i++) {
      for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
         shapes[i].mesh.positions[3*v+0] = (shapes[i].mesh.positions[3*v+0] - shiftX) * scaleX;
         assert(shapes[i].mesh.positions[3*v+0] >= -1.0 - epsilon);
         assert(shapes[i].mesh.positions[3*v+0] <= 1.0 + epsilon);
         shapes[i].mesh.positions[3*v+1] = (shapes[i].mesh.positions[3*v+1] - shiftY) * scaleY;
         assert(shapes[i].mesh.positions[3*v+1] >= -1.0 - epsilon);
         assert(shapes[i].mesh.positions[3*v+1] <= 1.0 + epsilon);
         shapes[i].mesh.positions[3*v+2] = (shapes[i].mesh.positions[3*v+2] - shiftZ) * scaleZ;
         assert(shapes[i].mesh.positions[3*v+2] >= -1.0 - epsilon);
         assert(shapes[i].mesh.positions[3*v+2] <= 1.0 + epsilon);
      }
   }
}

float sign(int p1x, int p1y, int p2x, int p2y, int p3x, int p3y)
{
	return (p1x - p3x) * (p2y - p3y) - (p2x - p3x) * (p1y - p3y);
}

bool PointInTriangle(int ptx, int pty, int v1x, int v1y, int v2x, int v2y, int v3x, int v3y)
{
	int d1, d2, d3;
	bool has_neg, has_pos;

	d1 = sign(ptx, pty, v1x, v1y, v2x, v2y);
	d2 = sign(ptx, pty, v2x, v2y, v3x, v3y);
	d3 = sign(ptx, pty, v3x, v3y, v1x, v1y);

	has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
}


float calculateColor(int area, int ptx, int pty, int v1x, int v1y, int v2x, int v2y)
{
	int one = v2x - v1x;
	int two = ptx - v1x;
	int three = v2y - v1y;
	int four = pty - v1y;
	float redArea = one * four - three * two;
	redArea = abs(redArea) / 2;
	float solution = redArea / area;
	return solution;
}
int calculateArea(int v1x, int v1y, int v1z, int v2x, int v2y, int v2z, int v3x, int v3y, int v3z)
{
	int one = v2x - v1x;
	int two = v3x - v1x;
	int three = v2y - v1y;
	int four = v3y - v1y;
	int solution = one * four - three * two; 
	solution = abs(solution) / 2;
	return solution;
}
int main(int argc, char **argv)
{
	/*if(argc < 3) {
      cout << "Usage: Assignment1 meshfile imagefile" << endl;
      return 0;
   }*/
	// OBJ filename
	/*string meshName(argv[1]);
	string imgName(argv[2]);*/

	//SWITCH BACK WHEN DONE !!!!!!!!!!!!!!!!!!!!!!!!!!!
	string meshName("tri.obj");
	string imgName("result.png");

	//set g_width and g_height appropriately!
	g_width = g_height = 100;

   //create an image
	auto image = make_shared<Image>(g_width, g_height);

	// triangle buffer
	vector<unsigned int> triBuf;
	// position buffer
	vector<float> posBuf;
	// Some obj files contain material information.
	// We'll ignore them for this assignment.
	vector<tinyobj::shape_t> shapes; // geometry
	vector<tinyobj::material_t> objMaterials; // material
	string errStr;
	
   bool rc = tinyobj::LoadObj(shapes, objMaterials, errStr, meshName.c_str());
	/* error checking on read */
	if(!rc) {
		cerr << errStr << endl;
	} else {
 		//keep this code to resize your object to be within -1 -> 1
   	resize_obj(shapes); 
		posBuf = shapes[0].mesh.positions;
		triBuf = shapes[0].mesh.indices;
	}
	cout << "Number of vertices: " << posBuf.size()/3 << endl;
	cout << "Number of triangles: " << triBuf.size()/3 << endl;

	//TODO add code to iterate through each triangle and rasterize it 
	for (int tri = 0; tri < triBuf.size() / 3; tri++)
	{
		int v1x;
		int v1y;
		int v1z;
		int v2x;
		int v2y;
		int v2z;
		int v3x;
		int v3y;
		int v3z;
		for (int x = 0; x <= posBuf.size(); x += 3)
		{
			if (x == 0)
			{
				v1x = (posBuf.at(x) + 1) * 50;
				v1y = (posBuf.at(x + 1) + 1) * 50;
				v1z = (posBuf.at(x + 2) + 1) * 50;
			}
			if (x == 3)
			{
				v2x = (posBuf.at(x) + 1) * 50;
				v2y = (posBuf.at(x + 1) + 1) * 50;
				v2z = (posBuf.at(x + 2) + 1) * 50;
			}
			if (x == 6)
			{
				v3x = (posBuf.at(x) + 1) * 50;
				v3y = (posBuf.at(x + 1) + 1) * 50;
				v3z = (posBuf.at(x + 2) + 1) * 50;
			}
		}
		/*cout << "Vertex 1" << endl;
		cout << v1x << endl;
		cout << v1y << endl;
		cout << v1z << endl;
		cout << "Vertex 2" << endl;
		cout << v2x << endl;
		cout << v2y << endl;
		cout << v2z << endl;
		cout << "Vertex 3" << endl;
		cout << v3x << endl;
		cout << v3y << endl;
		cout << v3z << endl;*/

		int area = calculateArea(v1x, v1y, v1z, v2x, v2y, v2z, v3x, v3y, v3z);

		int startx = min(v1x, v2x);
		startx = min(startx, v3x);
		int starty = max(v1y, v2y);
		starty = max(starty, v3y);


		int endx = max(v1x, v2x);
		endx = max(endx, v3x);
		int endy = min(v1y, v2y);
		endy = min(endy, v3y);

		int boxStart[2] = { startx, starty };
		int boxEnd[2] = { endx, endy };

		/*cout << "Start" << endl;
		cout << boxStart[0] << endl;
		cout << boxStart[1] << endl;

		cout << "End" << endl;
		cout << boxEnd[0] << endl;
		cout << boxEnd[1] << endl;*/
		for (int y = boxStart[1]; y >= boxEnd[1]; y--)
		{
			for (int x = boxStart[0]; x <= boxEnd[0]; x++)
			{
				if (x == image->getWidth() || y == image->getHeight())
				{
					continue;
				}
				if (PointInTriangle(x, y, v1x, v1y, v2x, v2y, v3x, v3y))
				{
					float green = calculateColor(area, x, y, v1x, v1y, v3x, v3y);
					float blue = calculateColor(area, x, y, v1x, v1y, v2x, v2y);
					float red = calculateColor(area, x, y, v2x, v2y, v3x, v3y);
					image->setPixel(x, y, 255*red, 255*green, 255*blue);
				}
			}
		}
	}
	//write out the image
   image->writeToFile(imgName);

	return 0;
}
