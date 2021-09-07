#pragma once
#include "GeoDetection.h"

namespace GeoDetection
{
//FEATURE-AVERAGING

		//Average-out normals around a given scale of core points. For entire cloud: set corepoints equal to cloud.
	pcl::PointCloud<pcl::Normal>::Ptr computeAverageNormals(const Cloud& geodetect,
		float scale, pcl::PointCloud<pcl::PointXYZ>::Ptr corepoints = nullptr);

	//Average-out scalar fields around a given scale of core points. For entire cloud: set corepoints equal to cloud.
	ScalarField computeAverageField(const Cloud& geodetect, const ScalarField& field, float scale,
		pcl::PointCloud<pcl::PointXYZ>::Ptr corepoints = nullptr);

//FEATURE-AVERAGING - MULTISCALE

	std::vector<ScalarField> getVolumetricDensitiesMultiscale(const Cloud& geodetect, const std::vector<float>& scales);

	//Average-out scalar fields around a given scale of core points, at numerous scales. 
	//For entire cloud: set corepoints equal to cloud.
	std::vector<ScalarField>
		computeAverageFieldMultiscale(const Cloud& geodetect, const ScalarField& field,
			const std::vector<float> scales, pcl::PointCloud<pcl::PointXYZ>::Ptr corepoints = nullptr);

//FEATURES

	//Gets a vector of curvatures, taken from normals.
	ScalarField NormalsToCurvature(const pcl::PointCloud<pcl::Normal>::Ptr normals);

	//Gets volumetric densities at a specified scale (scale)
	ScalarField getVolumetricDensities(const Cloud& geodetect, float scale);

	void getVegetationScore(ScalarField& vegetation_scores, float weight, ScalarField& curvatures, ScalarField& densities);

}
