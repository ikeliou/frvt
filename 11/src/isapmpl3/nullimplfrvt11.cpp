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

#include "tensorflow_mtcnn.hpp"
#include "mtcnn.hpp"
#include "comm_lib.hpp"
#include "utils.hpp"

using namespace std;
using namespace FRVT;
using namespace FRVT_11;

NullImplFRVT11::NullImplFRVT11() {}

NullImplFRVT11::~NullImplFRVT11() {}

ReturnStatus
NullImplFRVT11::initialize(const std::string &configDir)
{
	string model_fname = configDir+"/mtcnn_frozen_model.pb";
	cout<<endl<<"modelpath: "<<model_fname<<endl;
	load_graph(model_fname.c_str(),&(this->graph));
    return ReturnStatus(ReturnCode::Success);
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
	for (unsigned int i=0; i<faces.size(); i++) {
		cout<<"face"<<i<<endl;
		auto &image=faces[i];
		uint8_t *data=(uint8_t*)image.data.get();
		cout<<image.height<<" "<<image.width<<endl;
		cout<<(unsigned int)image.depth<<endl;
		cout<<image.size()<<endl;
		cv::Mat frame = cv::Mat(image.height,image.width,CV_8UC3,(unsigned char*)data);
		cv::Mat dframe = cv::Mat(image.height,image.width,CV_8UC3);
		cv::cvtColor(frame, dframe, CV_RGB2BGR);
		std::vector<face_box> face_list;

		cout<<"start detect: "<<i<<endl;
		mtcnn_detect(this->graph, dframe, face_list);
		cout<<"end detect: "<<i<<endl;

		if (face_list.size()==0) continue;
		const face_box &box = face_list[0];
		unsigned int xl=box.landmark.x[0];
		unsigned int yl=box.landmark.y[0];
		unsigned int xr=box.landmark.x[1];
		unsigned int yr=box.landmark.y[1];
		cout<<"eyes: "<<xl<<" "<<yl<<" "<<xr<<" "<<yr<<endl;
		eyeCoordinates.push_back(EyePair(true, true, xl, yl, xr, yr));

		if (ret.size()==0) {
			// net
			//matrix<rgb_pixel> face_chip;
			//cv::Mat corp_img=dframe(cv::Range(box.y0,box.y1),cv::Range(box.x0,box.x1));
			//dlib::assign_image(face_chip, cv_image<bgr_pixel>(corp_img));
			//matrix<float,0,1> face_descriptor = net(face_chip);
			//std::vector<float> a=std::vector<float>(face_descriptor.begin(), face_descriptor.end());
			//unsigned int n=a.size()*sizeof(float);
			//uint8_t* b = reinterpret_cast<uint8_t*>(a.data());
			//ret=std::vector<uint8_t>(b, b+n);
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


ReturnStatus
NullImplFRVT11::matchTemplates(
        const std::vector<uint8_t> &verifTemplate,
        const std::vector<uint8_t> &enrollTemplate,
        double &similarity)
{
    similarity = rand() % 1000 + 1;
    return ReturnStatus(ReturnCode::Success);
}

std::shared_ptr<Interface>
Interface::getImplementation()
{
    return std::make_shared<NullImplFRVT11>();
}





