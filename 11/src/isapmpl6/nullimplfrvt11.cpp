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

using namespace std;
using namespace FRVT;
using namespace FRVT_11;

NullImplFRVT11::NullImplFRVT11() {}

NullImplFRVT11::~NullImplFRVT11() {}

ReturnStatus
NullImplFRVT11::initialize(const std::string &configDir)
{
	//cout<< "initialize" <<endl;
	//cout<< "configDir " << configDir << endl;
	this->configDir = configDir;
   	this->h = kenxnet_init( this->configDir+"/models" );
	//cout<< this->h << endl;
    return ReturnStatus(ReturnCode::Success);
}

typedef struct Crop {
	float conf_rnet;
	float conf_onet;
	EyePair eye;
	cv::Rect rect;
	cv::Mat *mat;
} Crop;

ReturnStatus
NullImplFRVT11::createTemplate(
        const Multiface &faces,
        TemplateRole role,
        std::vector<uint8_t> &templ,
#ifdef debug
		cv::Mat &face_mat,
#endif
        std::vector<EyePair> &eyeCoordinates)
{
	//cout<<"createTemplate"<<endl;
	//cout<<"configDir: "<<this->configDir<<endl;
	std::vector<cv::Mat> mats(faces.size());
	vector<Crop> crops;
	for (unsigned int i=0; i<faces.size(); i++) {
		//cout<<"face"<<i<<endl;
		auto &image=faces[i];
		uint8_t *data=(uint8_t*)image.data.get();
		//cout<<image.height<<" "<<image.width<<endl;
		//cout<<(unsigned int)image.depth<<endl;
		//cout<<image.size()<<endl;
		if (image.depth == 24) {
			cv::Mat frame = cv::Mat(image.height,image.width,CV_8UC3,(unsigned char*)data);
			cv::cvtColor(frame, mats[i], CV_RGB2BGR);
		} else {
			cv::Mat frame = cv::Mat(image.height,image.width,CV_8UC1,(unsigned char*)data);
			cv::cvtColor(frame, mats[i], cv::COLOR_GRAY2BGR);
		}
		std::vector<face_box> face_list;
		//cout<< this->h <<endl;
		if (kenxnet_detectFace(this->h, mats[i], face_list)) {
			//cout<< "kenxnet_detectFace" << endl;
		}
		if (face_list.size()!=0) {
			//cout<<"get crops"<<endl;
			//cout<< confidence_rnet.size() << rectangles.size() << alignment.size() <<endl;
			Crop eye_crop;
			eye_crop.rect=cv::Rect(0,0,0,0);
			eye_crop.eye=EyePair(false, false, 0, 0, 0, 0);
			for (size_t j=0; j<face_list.size(); ++j) {
				struct face_box box = face_list[j];
				//cout << box.landmark.x[0] << " " << box.landmark.y[0] << endl;
				//cout << (int)box.x0 << " " << (int)box.y0 << endl;
				Crop crop;
				crop.rect=cv::Rect( (int)box.x0, (int)box.y0, int( box.x1-box.x0 ), int( box.y1-box.y0) );
				//cout << "crop:" << crop.rect <<endl;
				if (crop.rect.x<0) {
					crop.rect.x=0;
				}
				if (crop.rect.y<0) {
					crop.rect.y=0;
				}
				
				auto size = mats[i].size();
				if (crop.rect.x + crop.rect.width > size.width) {
					crop.rect.width=size.width-crop.rect.x;
				}
				if (crop.rect.y + crop.rect.height > size.height) {
					crop.rect.height=size.height-crop.rect.y;
				}
				//cout << crop.rect << endl;

				unsigned int xl=box.landmark.x[0];
				unsigned int yl=box.landmark.y[0];
				unsigned int xr=box.landmark.x[1];
				unsigned int yr=box.landmark.y[1];
				crop.eye=EyePair(true, true, xl, yl, xr, yr);
#ifdef debug
				crop.eye.x=crop.rect.x;
				crop.eye.y=crop.rect.y;
				crop.eye.w=crop.rect.width;
				crop.eye.h=crop.rect.height;
#endif
				crop.mat=&mats[i];
				crops.push_back(crop);
				if (crop.rect.area()>eye_crop.rect.area()) {
					eye_crop = crop;
				}
				//cout<<"eye: "<<xl<<" "<<yl<<" "<<xr<<" "<<yr<<endl;
			}
			eyeCoordinates.push_back(eye_crop.eye);
		} else {
			eyeCoordinates.push_back(EyePair(false, false, 0, 0, 0, 0));
		}
	}
	//cout<<"set eyes"<<endl;
	// set eyes
	std::vector<float> feature;
	if (crops.size()>0) {
		Crop ret_crop=crops[0];
		for (auto crop:crops) {
			if (crop.rect.area()>ret_crop.rect.area()) {
				ret_crop=crop;
			}
		}
		//cout<<"ret_crop: "<< ret_crop.rect <<endl;
		//cout<< "mat size: " << ret_crop.mat->size() << endl;
		//cout<< "mat w: " << ret_crop.mat->size().width << endl;
		//cout<< "mat h: " << ret_crop.mat->size().height << endl;
		cv::Mat mat = ret_crop.mat->operator()(ret_crop.rect);
#ifdef debug
		face_mat=mat;
#endif
		kenxnet_genFaceFeature(this->h, mat, feature);
/*
#ifdef debug
		eyeCoordinates.push_back(EyePair(false, false, ret_crop.rect.x, ret_crop.rect.y, ret_crop.rect.width, ret_crop.rect.height));
#endif
*/
	} else {
		//eyeCoordinates.push_back(EyePair(false, false, 0, 0, 0, 0));
		auto size=mats[0].size();
		kenxnet_genFaceFeature(this->h, mats[0], feature);
/*
#ifdef debug
		eyeCoordinates.push_back(EyePair(false, false, 0, 0, size.width, size.height));
#endif
*/
	}
	//cout<<"return feature"<<endl;	
	// return feature
	if (feature.size()>0) {
		std::vector<float> &a=feature;
		unsigned int n=a.size()*sizeof(float);
		uint8_t* b = reinterpret_cast<uint8_t*>(a.data());
		templ=std::vector<uint8_t>(b, b+n);
	} else {
		templ.resize(0);
	}
	//cout<<"templ.size() "<<templ.size()<<endl;
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
	cout << verifTemplate.size() << " " << enrollTemplate.size() << endl;
	if (verifTemplate.size()==0 || enrollTemplate.size()==0) {
		similarity = 0;
		return ReturnStatus(ReturnCode::MatchError);
	}
	std::vector<float> f1;
	std::vector<float> f2;
	toFloatVector(verifTemplate,f1);
	toFloatVector(enrollTemplate,f2);
	similarity = kenxnet_faceFeatureCompare(this->h, (float*)&f1[0], f1.size(), (float*)&f2[0], f2.size());
	cout << similarity << endl;
    return ReturnStatus(ReturnCode::Success);
}

std::shared_ptr<Interface>
Interface::getImplementation()
{
    return std::make_shared<NullImplFRVT11>();
	//return std::shared_ptr<NullImplFRVT11>(new NullImplFRVT11());
}





