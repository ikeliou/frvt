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
	float conf_rnet;
	float conf_onet;
	EyePair eye;
	cv::Rect rect;
	cv::Mat *mat;
} Crop;

static bool crop_onet_compare(const Crop &a, const Crop &b)
{
	return (a.conf_onet > b.conf_onet);
}

static bool crop_rnet_compare(const Crop &a, const Crop &b)
{
	return (a.conf_rnet > b.conf_rnet);
}

static bool crop_all_compare(const Crop &a, const Crop &b)
{
	return (a.conf_onet+a.conf_rnet > b.conf_onet+b.conf_rnet);
}

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
		if (image.depth == 24) {
			cv::Mat frame = cv::Mat(image.height,image.width,CV_8UC3,(unsigned char*)data);
			cv::cvtColor(frame, mats[i], CV_RGB2BGR);
		} else {
			cv::Mat frame = cv::Mat(image.height,image.width,CV_8UC1,(unsigned char*)data);
			cv::cvtColor(frame, mats[i], cv::COLOR_GRAY2BGR);
		}
		vector<cv::Rect> rectangles;
		std::vector<float> confidence_onet;
		//std::vector<float> confidence_rnet;
		std::vector<std::vector<cv::Point>> alignment;
		cout<<"start detect: "<<i<<endl;
		//cv::Mat img=cv::imread("/root/frvt2019/common/images/8bit.pgm");
		//this->mtcnn.detection(img, rectangles, confidence_onet, alignment);
		this->mtcnn.detection(mats[i], rectangles, confidence_onet, alignment);
		cout<<"end detect: "<<i<<endl;

		if (rectangles.size()!=0) {
			cout<<"get crops"<<endl;
			//cout<< confidence_rnet.size() << rectangles.size() << alignment.size() <<endl;
			for (size_t i=0; i<rectangles.size(); ++i) {
				Crop crop;
				crop.conf_onet=confidence_onet[i];
				//crop.conf_rnet=confidence_rnet[i];
				crop.rect=rectangles[i];
				if (crop.rect.x<0) {
					crop.rect.x=0;
				}
				if (crop.rect.y<0) {
					crop.rect.y=0;
				}
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
	// filter
	/*
	float threshold=0.3;
	vector<Crop> crops_f;
	while (threshold>0) {
		for (auto crop:crops) {
			if (crop.conf_rnet>=threshold)
				crops_f.push_back(crop);
		}
		if (crops_f.size()!=0) {
			crops=crops_f;
			break;
		}
		threshold-=0.1;
	}
	*/
	// set eyes
	std::vector<float> feature;
	if (crops.size()>0) {
		std::sort(crops.begin(), crops.end(), crop_onet_compare);
		Crop ret_crop=crops[0];
		for (auto crop:crops) {
			float diff=fabs(ret_crop.conf_onet - crop.conf_onet);
			cout<<"diff:" << diff <<endl;
			cout<<"area: "<< crop.rect.area() <<endl;
			cout<<"conf_onet: "<< crop.conf_onet <<endl;
			/*
			if (ret_crop.conf_onet<=0.8 && crop.conf_onet>=0.9) {
				ret_crop = crop;
				continue;
			}
			*/
			if (diff>0.05) break;
			float eye_diff = fabs(crop.eye.xleft-ret_crop.eye.xright)+fabs(crop.eye.yright-ret_crop.eye.yright);
			//if (eye_diff>1000. && crop.rect.area()>ret_crop.rect.area()) {
			//if (this->mtcnn.IoU(crop.rect, ret_crop.rect) > 0.9&& crop.rect.area()>ret_crop.rect.area()) {
			if (crop.rect.area()>ret_crop.rect.area()) {
				ret_crop=crop;
			}
			//else if (eye_diff>1000. && ret_crop.rect == (ret_crop.rect&crop.rect)) {
			//	ret_crop=crop;
			//}

			/*
			if (ret_crop.rect == (crop.rect&ret_crop.rect)) {
				if (ret_crop.conf_onet>crop.conf_onet && ret_crop.conf_onet>=0.8)
					continue;
				ret_crop=crop;
			} else {
				if (diff<=0.05) {
					if (crop.rect.area()>ret_crop.rect.area()) {
						ret_crop=crop;
					}
				} else {
					if (crop.conf_rnet>ret_crop.conf_rnet) {
						ret_crop=crop;
					}
				}
			}
			*/
		}
		cout<<"ret_crop: "<< ret_crop.rect <<endl;
		cout<<"ret_crop conf_onet: "<< ret_crop.conf_onet <<endl;
		//cout<<"ret_crop conf_rnet: "<< ret_crop.conf_rnet <<endl;
#if 0
		std::sort(crops.begin(), crops.end(), crop_onet_compare);
		Crop eye_crop=crops[0];
		for (auto crop:crops) {
			float diff=fabs(eye_crop.conf_onet - crop.conf_onet);
			cout<< "diff:" << diff <<endl;
			cout<< "area:"<< crop.rect.area() <<endl;
			cout<< "conf_onet:"<< crop.conf_onet <<endl;
			if (diff>0.001) {
				break;
			}
			if (crop.rect.area()>eye_crop.rect.area()) {
				eye_crop=crop;
			}
		}
		cout<<"eye_crop: "<< eye_crop.rect <<endl;
		cout<<"eye_crop conf_onet: "<< eye_crop.conf_onet <<endl;
		cout<<"eye_crop conf_rnet: "<< eye_crop.conf_rnet <<endl;
#endif
		Crop eye_crop=ret_crop;
		eyeCoordinates.push_back(eye_crop.eye);
		iSapGenerateFaceFeature(*ret_crop.mat, ret_crop.rect, feature);
#ifdef debug
		eyeCoordinates.push_back(EyePair(false, false, ret_crop.rect.x, ret_crop.rect.y, ret_crop.rect.width, ret_crop.rect.height));
#endif
	} else {
		eyeCoordinates.push_back(EyePair(false, false, 0, 0, 0, 0));
		auto size=mats[0].size();
		iSapGenerateFaceFeature(mats[0], cv::Rect(0,0,size.width,size.height), feature);
#ifdef debug
		eyeCoordinates.push_back(EyePair(false, false, 0, 0, size.width, size.height));
#endif
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





