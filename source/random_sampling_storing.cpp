#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <time.h> //will  be seed
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sstream>
#include <iostream>
/*
input:							big_picture(any filetype)
points_amount:			int
//(destination res:		x,y) optional -> maybe big_pic.res is best!

-> out: rsf file wiht header+points_amount of lines

getseed
srand(time(NULL));

for(int i; i<points_amount; i++)
{
	x= rand...
	y= rand..
	get pixelcolor	(auf/abrunden) //nicht nötig, um rundungsfehler wegzulassen int
	rsw_write(x,y, clr)
}

*/
using namespace cv;
//using namespace std;

int main(int argc, char** argv )
{
    if ( argc != 3 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }

    Mat image;
		std::string input= argv[1];
    image = imread( input, 1 );

    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }

		char *endptr;
		int samples_amount= strtol(argv[2], &endptr, 10);
		if (*endptr != '\0') {
			std::cout<<"usage: second parameter has to be an integer\n";
			return -1;
		}

		//input is fine
		srand(time(NULL));
		int SIZEX=image.cols;
		int SIZEY=image.rows;

		int irandx;
		int irandy;
		Vec3b color;

			std::string output_filename="from"+input;//+"_samples";
			//std::string test= std::to_string(4);//samples_amount);

		  std::fstream file("output_filename", std::ios::out);
		  file.clear();
		  file << "from: "<<input
		       << " "<<samples_amount<< "samples\n";


		for (int i=0; i<samples_amount; i++)
		{
			irandx= rand() % SIZEX;
		  irandy= rand() % SIZEY;
			color = image.at<Vec3b>(Point(irandx,irandy));
			file << irandx << ",";
			file << irandy << ";";
			file << (int)color[0] << ",";
			file << (int)color[1] << ",";
			file << (int)color[2] << ";\n";
			/*
			file <<"x:"<< irandx << ", ";
			file <<"y:"<<irandy << "; ";
			file <<"r:"<< (int)color[0] << ", ";
			file <<"g:"<< (int)color[1] << ", ";
			file <<"b:"<< (int)color[2] << ";\n";
			*/
		}

		file.close();
    return 0;
}
