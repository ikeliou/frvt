#ifndef _kenxnet_interfaace_h
#define _kenxnet_interfaace_h

#include <string>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


struct face_landmark
{
	float x[5];
	float y[5];
};

struct face_box
{
	float x0;
	float y0;
	float x1;
	float y1;

	float score;

	float regress[4];

	float px0;
	float py0;
	float px1;
	float py1;
	
	face_landmark landmark;  
};


#ifdef __cplusplus
   extern "C" {
#endif

void* kenxnet_init(std::string dataPath);
void  kenxnet_release( void *h );
bool  kenxnet_detectFace( void* h, cv::Mat &image, std::vector<face_box> &face_list );
bool  kenxnet_genFaceFeature( void* h, cv::Mat &image, std::vector<float> &feature );
float kenxnet_faceFeatureCompare( void* h, const float *source, int sourceSize, const float *target, int targetSize );

#ifdef __cplusplus
   }
#endif

#endif 