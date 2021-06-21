#include "importAscii.h"
#include "PclProcessing.h"

int main (int argc, char* argv[])
{
	std::cout << "-------------------DBSCAN-------------------\n" << std::endl;
//Program input declaration
	std::string filename;
	unsigned int min_points;
	double searchradius;

//Data containers declaration
	std::vector<XYZ> xyzpoints;  
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloudptr(new pcl::PointCloud<pcl::PointXYZ>); //smart pointer declaration to heap. 

//Usage: Expect 7 arguments; (1)dbscan.exe; (2)-i; (3)<filename>; (4)-m; (5)<min points>; (6)-d; (7)<min distance>
//OR (1)dbscan.exe (2) test (must have test.txt file in directory)

	if (argc != 7 && argc != 2) 
	{
		std::cerr << "Usage: " << argv[0] << "-i <filename> -m <min points> -d <min distance> " << std::endl;
		std::cerr << "Test Usage: dbscan test";
		return 1;	//exit program if the number of arguments is not met
	}

	if (argc == 2)
	{
		if (std::string(argv[1]) != "test")
		{
			std::cerr << "Usage: " << argv[0] << "-i <filename> -m <min points> -d <min distance> " << std::endl;
			std::cerr << "Test Usage: dbscan test";
		}
		std::cout << "Launching DBSCAN Test Dataset" << std::endl;
		filename = "test.txt";
		min_points = 12;
		searchradius = 0.3;
	}
	else
	{
		for (int i = 1; i < argc; i++)
		{
			if (std::string(argv[i]) == "-i") {
				filename = argv[i + 1];
			}
			else if (std::string(argv[i]) == "-m") {
				min_points = atoi(argv[i + 1]);
			}

			else if (std::string(argv[i]) == "-d") {
				searchradius = atof(argv[i + 1]);
			}
		}
	}

//Import data into xyzpoints
	//std::cout << "File: " << filename << '\n' << std::endl;
	//readXYZ(filename, xyzpoints); //read file and store into fullpoints
	//unsigned int num_points = xyzpoints.size();


	return 0;
}