/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility  whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <cstring>
#include <cstdlib>

#include "nullimplfrvt11.h"

#include "sdk/libFaceRecognition.h"

using namespace std;
using namespace FRVT;
using namespace FRVT_11;

NullImplFRVT11::NullImplFRVT11() {}

NullImplFRVT11::~NullImplFRVT11() {}

ReturnStatus
NullImplFRVT11::initialize(const std::string &configDir)
{
    vector<string> model_file = {
            configDir+"/model/det1.prototxt",
            configDir+"/model/det2.prototxt",
            configDir+"/model/det3.prototxt"
//            "../model/det4.prototxt"
    };

    vector<string> trained_file = {
            configDir+"/model/det1.caffemodel",
            configDir+"/model/det2.caffemodel",
            configDir+"/model/det3.caffemodel"
//            "../model/det4.caffemodel"
    };
	this->mtcnn = MTCNN(model_file, trained_file);
	iSapInit( configDir+"/data" );
    return ReturnStatus(ReturnCode::Success);
}

typedef struct Crop {
	float conf;
	EyePair eye;
	cv::Rect rect;
	cv::Mat *mat;
} Crop;

ReturnStatus
NullImplFRVT11::createTemplate(
        const Multiface &faces,
        TemplateRole role,
        std::vector<uint8_t> &templ,
        std::vector<EyePair> &eyeCoordinates)
{
	cout<<"createTemplate"<<endl;

	std::vector<cv::Mat> mats(faces.size());
	vector<Crop> crops;
	for (unsigned int i=0; i<faces.size(); i++) {
		cout<<"face"<<i<<endl;
		auto &image=faces[i];
		uint8_t *data=(uint8_t*)image.data.get();
		cout<<image.height<<" "<<image.width<<endl;
		cout<<(unsigned int)image.depth<<endl;
		cout<<image.size()<<endl;
		cv::Mat frame = cv::Mat(image.height,image.width,CV_8UC3,(unsigned char*)data);
		//mats.push_back(cv::Mat(image.height,image.width,CV_8UC3));
		cv::cvtColor(frame, mats[i], CV_RGB2BGR);
		
		vector<cv::Rect> rectangles;
		std::vector<float> confidence;
		std::vector<std::vector<cv::Point>> alignment;
		cout<<"start detect: "<<i<<endl;
		this->mtcnn.detection(mats[i], rectangles, confidence, alignment);
		cout<<"end detect: "<<i<<endl;

		if (rectangles.size()!=0) {
			cout<<"get crops"<<endl;
			for (size_t i=0; i<confidence.size(); ++i) {
				Crop crop;
				crop.conf=confidence[i];
				crop.rect=rectangles[i];
				unsigned int xl=alignment[i][0].x;
				unsigned int yl=alignment[i][0].y;
				unsigned int xr=alignment[i][1].x;
				unsigned int yr=alignment[i][1].y;
				crop.eye=EyePair(true, true, xl, yl, xr, yr);
				crop.mat=&mats.back();
				crops.push_back(crop);
				cout<<"eye: "<<xl<<" "<<yl<<" "<<xr<<" "<<yr<<endl;
				
			}
		}
	}
	cout<<"set eyes"<<endl;	
	// set eyes
	std::vector<float> feature;
	if (crops.size()>0) {
		Crop ret_crop=crops[0];
		for (auto &crop:crops) {
			//if (crop.rect.area()>ret_crop.rect.area()) {
			if (crop.conf>ret_crop.conf) {
				ret_crop=crop;
			}
		}
		eyeCoordinates.push_back(ret_crop.eye);
		iSapGenerateFaceFeature(*ret_crop.mat, ret_crop.rect, feature);
	} else {
		eyeCoordinates.push_back(EyePair(false, false, 0, 0, 0, 0));
		auto size=mats[0].size();
		iSapGenerateFaceFeature(mats[0], cv::Rect(0,0,size.width,size.height), feature);
	}

	cout<<"return feature"<<endl;	
	// return feature
	if (feature.size()>0) {
		std::vector<float> &a=feature;
		unsigned int n=a.size()*sizeof(float);
		uint8_t* b = reinterpret_cast<uint8_t*>(a.data());
		templ=std::vector<uint8_t>(b, b+n);
	} else {
		templ.resize(0);
	}
	cout<<"templ.size() "<<templ.size()<<endl;
	if (templ.size()==0) {
		return ReturnStatus(ReturnCode::TemplateCreationError);
	}

    return ReturnStatus(ReturnCode::Success);
}

void toFloatVector(const std::vector<uint8_t> &templData, std::vector<float> &feature) {
	const float* floatArray = reinterpret_cast<const float*>(templData.data());
	unsigned int count = templData.size()/sizeof(float);
	feature = std::vector<float>(floatArray, floatArray+count);
}

ReturnStatus
NullImplFRVT11::matchTemplates(
        const std::vector<uint8_t> &verifTemplate,
        const std::vector<uint8_t> &enrollTemplate,
        double &similarity)
{
	if (verifTemplate.size()==0 || enrollTemplate.size()==0) {
		similarity = 0;
		return ReturnStatus(ReturnCode::MatchError);
	}
	std::vector<float> f1;
	std::vector<float> f2;
	toFloatVector(verifTemplate,f1);
	toFloatVector(enrollTemplate,f2);
	float score;
	iSapCompareFaceFeature(f1,f2,score);
	similarity = score;
    return ReturnStatus(ReturnCode::Success);
}

std::shared_ptr<Interface>
Interface::getImplementation()
{
    return std::make_shared<NullImplFRVT11>();
}





