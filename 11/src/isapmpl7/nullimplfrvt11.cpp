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
	cout<< "initialize" <<endl;
	//cout<< "configDir " << configDir << endl;
	this->configDir = configDir;
   	this->h = kenxnet_init( this->configDir+"/models" );
    init( this->configDir+"/retina.param", this->configDir+"/retina.bin" );
    lffd_init( this->configDir );
	//cout<< this-> h << endl;
    return ReturnStatus(ReturnCode::Success);
}

typedef struct Crop {
	float conf;
	EyePair eye;
	cv::Rect rect;
	cv::Mat *mat;
    std::vector<cv::Point2f> pts;
    int error;
    cv::Mat trans_mat;
    int dir;
    int pose_type;
    bool pose_ret;
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
    //this->detector.Init( this->configDir+"/model/face.param", this->configDir+"/model/face.bin" );
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
		//std::vector<face_box> face_list;
		//cout<< this->h <<endl;
		
		cv::Mat &img=mats[i];
        std::vector<facebox> boxes;
		cout<< "start detecting" << endl;
        detect(img, boxes);
		cout<< "end detecting" << endl;

		//if (kenxnet_detectFace(this->h, mats[i], face_list)) {
			//cout<< "kenxnet_detectFace" << endl;
		//}
		if (boxes.size()!=0) {
			//cout<<"get crops"<<endl;
			//cout<< confidence_rnet.size() << rectangles.size() << alignment.size() <<endl;
			Crop eye_crop;
			eye_crop.rect=cv::Rect(0,0,0,0);
			eye_crop.eye=EyePair(false, false, 0, 0, 0, 0);
			for (size_t j=0; j<boxes.size(); ++j) {
				auto box = boxes[j].rect;
				auto pts = boxes[j].keypoints;
            	cv::Rect rect(box.x, box.y, box.width, box.height);
				//cout << box.landmark.x[0] << " " << box.landmark.y[0] << endl;
				//cout << (int)box.x0 << " " << (int)box.y0 << endl;

				Crop crop;
				crop.rect=fit_rect(mats[i], rect);
				crop.conf=boxes[j].score;
				//cout << "crop:" << crop.rect <<endl;
				//cout << crop.rect << endl;

				unsigned int xl=pts[0].x;
				unsigned int yl=pts[0].y;
				unsigned int xr=pts[1].x;
				unsigned int yr=pts[1].y;
                crop.pts=pts;
				crop.eye=EyePair(true, true, xl, yl, xr, yr);
#ifdef debug
				crop.eye.x=crop.rect.x;
				crop.eye.y=crop.rect.y;
				crop.eye.w=crop.rect.width;
				crop.eye.h=crop.rect.height;
				crop.eye.conf=crop.conf;
                bool ret = check_large_pose(crop.pts, crop.rect, &crop.eye.pose_type);
                crop.eye.pose_ret = ret ? 1 : 0;
#endif
				crop.mat=&mats[i];
				//crops.push_back(crop);
				if (crop.rect.area()>eye_crop.rect.area()) {
					eye_crop = crop;
				}
				//cout<<"eye: "<<xl<<" "<<yl<<" "<<xr<<" "<<yr<<endl;
			}
			eyeCoordinates.push_back(eye_crop.eye);
			crops.push_back(eye_crop);
		} else {
			eyeCoordinates.push_back(EyePair(false, false, 0, 0, 0, 0));
		}
	}
	cout<<"set eyes"<<endl;
	// set eyes
	std::vector<float> feature;
	if (crops.size()>0) {
		for (auto &crop:crops) {
            cout << "test 1" << endl;
            auto info = get_face_info(*crop.mat, crop.pts);
            cout << "test 2" << endl;
            cout << info.errors.size() << endl;
            for (auto error:info.errors) {
                cout << error << " ";
            }
            cout << endl;
            crop.dir = std::min_element(info.errors.begin(), info.errors.end()) - info.errors.begin();
            cout << "test 3" << endl;
            crop.trans_mat = info.trans_mats[crop.dir];
            crop.error = info.errors[2];
            crop.pose_ret = check_large_pose(crop.pts, crop.rect, &crop.pose_type);
		}
        cout << "test 4" << endl;
		Crop ret_crop=crops[0];
		for (auto &crop:crops) {
            /*
            if (crop.dir!=ret_crop.dir) {
                auto ret_v=std::max(ret_crop.dir,2)-std::min(ret_crop.dir,2);
                auto v=std::max(crop.dir,2)-std::min(crop.dir,2);
                if (v<ret_v) {
                    ret_crop=crop;
                }
            } else {
                if (crop.rect.area()>ret_crop.rect.area()) {
                    ret_crop=crop;
                }
            }
            */
            /*
            if (!crop.pose_ret) continue;

            int lv_new=(crop.pose_type+1)/2;
            int lv_old=(ret_crop.pose_type+1)/2;
            if (lv_new==lv_old && crop.rect.area()>ret_crop.rect.area()) {
                ret_crop=crop;
            }
            if (lv_new<lv_old) {
                ret_crop=crop;
            }
            */
            /*
            if (crop.error < ret_crop.error) {
                ret_crop = crop;
            }
            */
            int conf=(int)(crop.conf*100);
            int ret_conf=(int)(ret_crop.conf*100);
            if (conf > ret_conf) {
                ret_crop = crop;
            }
            if (conf == ret_conf && crop.rect.area()>ret_crop.rect.area()) {
                ret_crop = crop;
            }

        }
        cv::Mat mat;
        //mat=ret_crop.mat->operator()(ret_crop.rect);
        cv::Mat aligned_mat;
        //align(*ret_crop.mat, ret_crop.pts, &aligned_mat);
        //align_with_trans(*ret_crop.mat, ret_crop.trans_mat, &aligned_mat);
        aligned_mat = align_img(*ret_crop.mat, ret_crop.pts);
        std::vector<facebox> boxes;
        detect(aligned_mat, boxes);
        if (boxes.size()!=0) {
	        cv::Rect rect=boxes[0].rect;
            for (auto box:boxes) {
                if (box.rect.area()>rect.area()) {
                    rect=box.rect;
                }
            }
            auto size=aligned_mat.size();
            rect=fit_rect(aligned_mat, rect);
            std::cout << rect << std::endl;
            std::cout << size << std::endl;
            mat=aligned_mat.operator()(rect);
        } else {
            mat=ret_crop.mat->operator()(ret_crop.rect);
        }
#ifdef debug
        //align(*ret_crop.mat, ret_crop.pts, &face_mat, 1);
        //align_with_trans(*ret_crop.mat, ret_crop.trans_mat, &face_mat);
		face_mat=mat;
#endif
		kenxnet_genFaceFeature(this->h, mat, feature);
	} else {
		auto size=mats[0].size();
		kenxnet_genFaceFeature(this->h, mats[0], feature);
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





