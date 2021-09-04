#include "readascii_core.h"
#include "readascii.h"
#include <fast_float/fast_float.h> // for fast parsing

namespace GeoDetection
{
	//Helpers
	void
		reserveCloudSpace(const char* fname, bool header_present,
			pcl::PointCloud<pcl::PointXYZ>::Ptr cloudptr)
	{
		size_t num_points = getLineCount(fname);
		if (num_points < 2) {
			GD_ERROR("Needs more than 2 points to create a point cloud object");
			return;
		}

		if (header_present) { num_points--; }

		cloudptr->points.reserve(num_points);
		GD_TRACE(":: Number of points: {0}", num_points);
	}

	//Parses the file and returns a GeoDetection Cloud. Currently only imports scalar fields and XYZ coordinates.
	//Option for normals will be added later.
	Cloud 
		parseData(FILE* file, size_t num_points, size_t num_columns, char* buf, const int64_t BUFFER_SIZE)
	{
		GD_CORE_TRACE(":: Parsing ascii file...");
		size_t num_fields = num_columns - 3;

		//Get pointers to all of the member variables, which we will fill. 
		Cloud geodetect;
		auto cloud = geodetect.cloud();
		auto normals = geodetect.normals();
		auto scalarfields = geodetect.scalarfields();

		//Reserve space, considering num_columns and num_points
		cloud->points.reserve(num_points);
		normals->points.reserve(num_points);

		scalarfields->reserve(num_fields); // if normals too, -6. --> add this later. 
		for (size_t i = 0; i < num_fields; i++) { scalarfields[i].reserve(num_points); }

		//temporary containers for parsing and pushing into classes
		pcl::PointXYZ xyz;
		pcl::Normal n;
		float sf; 
		
		//Fill buffer until the file is read.
		while (size_t bytes_read = fread(&buf, 1, BUFFER_SIZE, file)) {

			char* p = buf; //pointer location in buffer

			if (bytes_read == std::numeric_limits<size_t>::max())
				GD_ERROR("Read failed!");
			if (!bytes_read) //zero bytes read (at end of file)
				break;

			//p_next is the pointer to the next '\n' in the buffer
			for (char* p_next = buf; (p_next = (char*)memchr(p_next, '\n', (buf + bytes_read) - p_next)); p_next++) {
				
				//Parse first 3 columns (xyz)
				auto result = fast_float::from_chars(p, p_next, xyz.x);
				result = fast_float::from_chars(result.ptr + 1, p_next, xyz.y);
				fast_float::from_chars(result.ptr + 1, p_next, xyz.z);
				cloud->points.push_back(xyz);

				for (size_t i = 0; i < num_fields; i++)
				{
					result = fast_float::from_chars(result.ptr + 1, p_next, sf);
					scalarfields[i].push_back(sf);
				}

				p = p_next + 1;
			}
			// CASE: The xyz point is split across the buffer. Reset the file location to include it at the beginning of the next buffer fill.
			if (bytes_read == BUFFER_SIZE) {
				size_t offset = (buf + bytes_read) - p; //number of bytes from end of buffer to the last read `\n`. 
				fseek(file, (long)-offset, SEEK_CUR); //move file pointer to the beginning of the point for next buffer fill.
				continue;
			}
			// EOF CASE: The last line in the file didn't terminate with \n
			else if (cloud->points.size() < num_points) {
				char* p_end; //pointer which lands on the delimeters.

				xyz.x = strtod(p, &p_end);
				xyz.y = strtod(p_end + 1, &p_end);
				xyz.z = strtod(p_end + 1, &p_end);
				cloud->points.push_back(xyz);

				for (size_t i = 0; i < num_fields; i++) {scalarfields[i].push_back(strtod(p_end + 1, &p_end));}
			}
		}
		return geodetect;
	}

	Cloud 
		ReadAscii::import()
	{
			GD_TITLE("Ascii Data Import");
			auto start = GeoDetection::Time::getStart();

			//initialize buffer
			static const int64_t BUFFER_SIZE = 16 * 1024;
			char buf[BUFFER_SIZE + 1];

			//Get number of points in the file, and reduce by one if header is present.
			bool header_present = hasHeader(m_filename);
			size_t num_points = getLineCount(m_filename) - (int)header_present;
			GD_TRACE(":: Number of points: {0}", num_points);

			//Get number of columns in the file
			size_t num_columns = getNumColumns(m_filename);
			GD_TRACE(":: Number of columns: {0}",  num_columns);

			//Open the c-style file for reading
			GD_TRACE(":: Opening File: {0}...", m_filename);
			FILE* file;
			errno_t err;

			if ((err = fopen_s(&file, m_filename, "r")) != 0)
			{
				GD_ERROR("Failed to open file: {0} \n", m_filename);
				std::exit(EXIT_FAILURE);
			}

			//If there's a header, skip the first line.
			if (header_present) {
				char* str = fgets(buf, BUFFER_SIZE, file);
			}

			//Parse the data and create a GeoDetection object
			Cloud geodetect = parseData(file, num_points, num_columns, buf, BUFFER_SIZE);

			fclose(file);
			GD_WARN("--> Data import time: {0} ms", GeoDetection::Time::getDuration(start));

			return geodetect;
	}

}