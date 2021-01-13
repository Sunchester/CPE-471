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

float sign(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y)
{
	return (p1x - p3x) * (p2y - p3y) - (p2x - p3x) * (p1y - p3y);
}

bool PointInTriangle(float ptx, float pty, float v1x, float v1y, float v2x, float v2y, float v3x, float v3y)
{
	float d1, d2, d3;
	bool has_neg, has_pos;

	d1 = sign(ptx, pty, v1x, v1y, v2x, v2y);
	d2 = sign(ptx, pty, v2x, v2y, v3x, v3y);
	d3 = sign(ptx, pty, v3x, v3y, v1x, v1y);

	has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
}


float calculateColor(float area, float ptx, float pty, float v1x, float v1y, float v2x, float v2y)
{
	float one = v2x - v1x;
	float two = ptx - v1x;
	float three = v2y - v1y;
	float four = pty - v1y;
	float redArea = one * four - three * two;
	redArea = (abs(redArea)) / 2.0;
	float solution = (redArea / area);
	return solution;
}
float calculateArea(float v1x, float v1y, float v1z, float v2x, float v2y, float v2z, float v3x, float v3y, float v3z)
{
	float one = v2x - v1x;
	float two = v3x - v1x;
	float three = v2y - v1y;
	float four = v3y - v1y;
	float solution = one * four - three * two; 
	solution = abs(solution) / 2;
	return solution;
}
class v3d
{
public:
	float x, y, z;
};
float calculateRange(float z)
{
	float result = (((z + 1) * 255) / 2);
	return result;
}
int main(int argc, char **argv)
{

	// Inputs and Outputs
	string meshName("bunny.obj");
	string imgName("resultRed2.png");
	//set g_width and g_height appropriately!
	g_width = g_height = 100;
   //create an image
	auto image = make_shared<Image>(g_width, g_height);

	auto zbuffer = make_shared<Image>(g_width, g_height);
	for (int y = 0; y < g_height; y++)
	{
		for (int x = 0; x < g_width; x++)
		{
			zbuffer->setPixel(x, y, 0, 0, 0);
		}

	}
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
	
	v3d* vertices = new v3d[(posBuf.size()/3)];
	
	int arrayIndex = 0;
	for (int ind = 0; ind < posBuf.size(); ind += 3)
	{
		vertices[arrayIndex].x = posBuf[ind + 0];
		vertices[arrayIndex].y = posBuf[ind + 1];
		vertices[arrayIndex].z = posBuf[ind + 2];
		arrayIndex += 1;
	}
	//TODO add code to iterate through each triangle and rasterize it
	for (int i = 0; i < triBuf.size(); i+=3)
	{
		v3d a, b, c;
		int v1x, v1y, v1z, v2x, v2y, v2z, v3x, v3y, v3z;

		a = vertices[triBuf[i + 0]];
		b = vertices[triBuf[i + 1]];
		c = vertices[triBuf[i + 2]];
	
		v2x = (b.x + 1) * 50;
		v3x = (c.x + 1) * 50;
		v1y = (a.y + 1) * 50;
		v1x = (a.x + 1) * 50;
		v2y = (b.y + 1) * 50;
		v3y = (c.y + 1) * 50;
		v1z= (a.z + 1) * 50;
		v2z= (b.z + 1) * 50;
		v3z= (c.z + 1) * 50;


		float area = calculateArea(a.x, a.y, a.z, b.x, b.y, b.z, c.x, c.y, c.z);
		/*double area = calculateArea(v1x, v1y, v1z, v2x, v2y, v2z, v3x, v3y, v3z);*/

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

	
		for (int y = boxStart[1]; y >= boxEnd[1]; y--)
		{
			for (int x = boxStart[0]; x <= boxEnd[0]; x++)
			{
				if (x == image->getWidth() || y == image->getHeight())
				{
					continue;
				}
				float xfloat = (x / 50.0) - 1.0;
				float yfloat = (y / 50.0) - 1.0;
				if (PointInTriangle(xfloat, yfloat, a.x, a.y, b.x, b.y, c.x, c.y))
				{
					
					float red1 = calculateColor(area, xfloat, yfloat, a.x, a.y, c.x, c.y);
					float red2 = calculateColor(area, xfloat, yfloat, a.x, a.y, b.x, b.y);
					float red3 = calculateColor(area, xfloat, yfloat, b.x, b.y, c.x, c.y);
					float convertz1 = calculateRange(a.z);
					float convertz2 = calculateRange(b.z);
					float convertz3 = calculateRange(c.z);
					float total = convertz2*red1 + convertz3*red2 + convertz1*red3;
					float pixelColor = zbuffer->getPixel(x, y);
					if (total >= pixelColor)
					{
						zbuffer->setPixel(x, y, total, 0, 0);
						image->setPixel(x, y, total, 0, 0);
					}

				}
			}
		}
	}
	//write out the image
    image->writeToFile(imgName);
   
   return 0;
}
