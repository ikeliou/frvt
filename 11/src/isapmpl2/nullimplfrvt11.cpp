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
    deserialize(configDir+"/shape_predictor_68_face_landmarks.dat") >> this->sp;
	deserialize(configDir+"/dlib_face_recognition_resnet_model_v1.dat") >> this->net;
    return ReturnStatus(ReturnCode::Success);
}

void ImageCopyToDLib(const FRVT::Image &image, dlib::array2d<rgb_pixel> &img) {
	char *data=(char*)image.data.get();

	for (unsigned int h=0; h<image.height; h++) {
		for (unsigned int w=0; w<image.width; w++) {
			unsigned int i=(h*image.width + w)*3;
			img[h][w].red=data[i];
			img[h][w].green=data[i+1];
			img[h][w].blue=data[i+2];
		}
	}
}

ReturnStatus
NullImplFRVT11::createTemplate(
        const Multiface &faces,
        TemplateRole role,
        std::vector<uint8_t> &templ,
        std::vector<EyePair> &eyeCoordinates)
{
	cout<<"createTemplate"<<endl;
	std::vector<uint8_t> ret;
    frontal_face_detector detector = get_frontal_face_detector();
	for (unsigned int i=0; i<faces.size(); i++) {
		auto &image=faces[i];
		array2d<rgb_pixel> img=array2d<rgb_pixel>(image.height,image.width);
		ImageCopyToDLib(image, img);
		std::vector<rectangle> dets = detector(img);
		if (dets.size()<=0) {
			eyeCoordinates.push_back(EyePair(false, false, 0, 1, 2, 3));
			cout << "no detect!" << endl;
			continue;
		}
		auto face=dets[0];
		full_object_detection shape = sp(img, face);
		cout << "number of parts: "<< shape.num_parts() << endl;
		cout << "pixel position of first part:  " << shape.part(36) << endl;
		cout << "pixel position of second part: " << shape.part(42) << endl;
		dlib::point p1=shape.part(36);
		dlib::point p2=shape.part(42);
		unsigned int xl=p1.x();
		unsigned int yl=p1.y();
		unsigned int xr=p2.x();
		unsigned int yr=p2.y();

		eyeCoordinates.push_back(EyePair(true, true, xl, yl, xr, yr));

		if (ret.size()==0) {
			// net
			matrix<rgb_pixel> face_chip;
			extract_image_chip(img, get_face_chip_details(shape,150,0.25), face_chip);
			matrix<float,0,1> face_descriptor = net(face_chip);
			std::vector<float> a=std::vector<float>(face_descriptor.begin(), face_descriptor.end());
			unsigned int n=a.size()*sizeof(float);
			uint8_t* b = reinterpret_cast<uint8_t*>(a.data());
			ret=std::vector<uint8_t>(b, b+n);
		}
	}
	templ.resize(ret.size());
    templ=ret;
	cout<<"templ.size() "<<templ.size()<<endl;
	if (templ.size()==0) {
		return ReturnStatus(ReturnCode::TemplateCreationError);
	}

    return ReturnStatus(ReturnCode::Success);
}

void toDlibFeatrue(const std::vector<uint8_t> &templData, matrix<float,0,1> &ma) {
	const float* floatArray = reinterpret_cast<const float*>(templData.data());
	unsigned int count = templData.size()/sizeof(float);
	cout<<"data size: "<<templData.size()<<endl;
	cout<<"count: "<<count<<endl;
	ma.set_size(count);
	for (unsigned int i=0; i<count; ++i) {
		ma(i)=floatArray[i];
	}
	cout<<"toDlibFeatrue finish"<<endl;
}

ReturnStatus
NullImplFRVT11::matchTemplates(
        const std::vector<uint8_t> &verifTemplate,
        const std::vector<uint8_t> &enrollTemplate,
        double &similarity)
{
	if (verifTemplate.size()==0 || enrollTemplate.size()==0) {
		similarity = 0;
		return ReturnStatus(ReturnCode::Success);
	}
	matrix<float,0,1> fd1;
	matrix<float,0,1> fd2;
	toDlibFeatrue(verifTemplate, fd1);
	toDlibFeatrue(enrollTemplate, fd2);
	float score=length(fd1-fd2);
	score=1.-score;
    similarity = score;
    return ReturnStatus(ReturnCode::Success);
}

std::shared_ptr<Interface>
Interface::getImplementation()
{
    return std::make_shared<NullImplFRVT11>();
}





