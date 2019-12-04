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
    retina_init( this->configDir+"/retina.param", this->configDir+"/retina.bin" );
    lffd_init( this->configDir );
    rfb_init(this->configDir+"/face.param", this->configDir+"/face.bin");
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
/*
static bool abs_compare(int a, int b)
{
    return (std::abs(a) < std::abs(b));
}
*/
static bool area_compare (facebox const &b1, facebox const &b2) {
    return (b1.rect.area()<b2.rect.area());
}
void setup_crop(Crop &crop, facebox face_box, bool with_score, bool with_landmark) {
    auto box = face_box.rect;
    auto pts = face_box.keypoints;
    cv::Rect rect(box.x, box.y, box.width, box.height);
	cout<<"a"<<endl;
    //Crop crop;
    crop.rect=fit_rect(*crop.mat, rect);
	cout<<"a"<<endl;
    if (with_score) {
        crop.conf=face_box.score;
        crop.eye.conf=face_box.score;
        printf("%f,%f\n",crop.conf,crop.eye.conf);
    }
    if (with_landmark) {
	    cout<<"a "<< pts.size() << endl;
        unsigned int xl=pts[0].x;
        unsigned int yl=pts[0].y;
        unsigned int xr=pts[1].x;
        unsigned int yr=pts[1].y;
	    cout<<"a"<<endl;
	    cout<<"a"<<endl;
        crop.pts=pts;
        crop.eye.isLeftAssigned=true;
        crop.eye.isRightAssigned=true;
        crop.eye.xleft=xl;
        crop.eye.yleft=yl;
        crop.eye.xright=xr;
        crop.eye.yright=yr;
        //crop.eye=EyePair(true, true, xl, yl, xr, yr);
#ifdef debug
        crop.eye.x=crop.rect.x;
        crop.eye.y=crop.rect.y;
        crop.eye.w=crop.rect.width;
        crop.eye.h=crop.rect.height;
        bool ret = check_large_pose(crop.pts, crop.rect, &crop.eye.pose_type);
        crop.eye.pose_ret = ret ? 1 : 0;
#endif
    }
}

facebox get_max_face(vector<facebox> boxes) {
    auto it = std::max_element(boxes.begin(), boxes.end(), area_compare
            //[] (facebox const b1, facebox const b2) {
            //    (b1.rect.area()<b2.rect.area());
            //}
            );
    int j = std::distance(boxes.begin(), it);
    return boxes[j];
}

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
        Crop crop;
        crop.conf=-1.;
        crops.push_back(crop);
    }
    for (unsigned i=0; i<faces.size(); i++) {
		eyeCoordinates.push_back(EyePair(false, false, 0, 0, 0, 0));
    }
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
		
		//cv::Mat &img=mats[i];
        std::vector<facebox> boxes;
		cout<< "start detecting" << endl;
        //retina_detect(mats[i], boxes);
        retina_detect(mats[i], boxes);
		cout<< "end detecting" << endl;
        //auto it = std::max_element(boxes.begin(), boxes.end(), area_compare
            //[] (facebox const b1, facebox const b2) {
            //    (b1.rect.area()<b2.rect.area());
            //}
        //);
        //int j = std::distance(boxes.begin(), it);
        crops[i].mat=&mats[i];
		if (boxes.size()!=0) {
            auto face_box = get_max_face(boxes);
            setup_crop(crops[i],face_box,false,true);
            crops[i].conf=0;
			eyeCoordinates[i]=crops[i].eye;
			//crops.push_back(crop);
            //crops[i]=crop;
		}

        std::vector<facebox> lffd_boxes;
		cout<< "start lffd detecting" << endl;
        rfb_detect(mats[i], lffd_boxes);
		cout<< "end lffd detecting" << endl;
        cout<< lffd_boxes.size() << endl;
		if (lffd_boxes.size()!=0) {
            auto face_box = get_max_face(lffd_boxes);
            cout<< "score:" <<endl;
            cout<< face_box.score << endl;
            //setup_crop(crops[i],face_box,true,false);
            setup_crop(crops[i],face_box,true,true);
            cout<< "conf:" <<endl;
            cout<< crops[i].eye.conf <<endl;

			eyeCoordinates[i]=crops[i].eye;
        }
        /*
        auto rect=crops[i].rect;
        int rwidth  = rect.width;
        int rheight = rect.height;
        cout << "rect:" << rect << endl;
        rect.width = rect.width * 0.8;
        rect.height = rect.height * 0.8;        
        rect.x += (rwidth - rect.width) / 2;
        rect.y += (rheight - rect.height) / 2;
        cout << "rect:" << rect << endl;

        if (boxes.size()!=0) {
            float x1=crops[i].pts[0].x;
            float y1=crops[i].pts[0].y;
            float x2=crops[i].pts[0].x;
            float y2=crops[i].pts[0].y;
            for (auto pt:crops[i].pts) {
                if (pt.x<x1) x1=pt.x;
                if (pt.y<y1) y1=pt.y;
                if (pt.x>x2) x2=pt.x;
                if (pt.y>y2) y2=pt.y;
            }
            //auto px=(x2-x1)/3.;
            //auto py=(y2-y1)/3.;
            //crops[i].rect=cv::Rect(x1-px,y1-py,x2-x1+px*2,y2-y1+py*2);
            //crops[i].rect=fit_rect(*crops[i].mat, crops[i].rect);
            auto p1=rect.tl();
            auto p2=rect.br();
            if (p1.x>x1) p1.x=x1;
            if (p1.y>y1) p1.y=y1;
            if (p2.x<x2) p2.x=x2;
            if (p2.y<y2) p2.y=y2;
            rect=cv::Rect(p1,p2);
        }
        crops[i].rect=fit_rect(*crops[i].mat, rect);
        cout << "rect:" << rect << endl;
        */
	}
	cout<<"set eyes"<<endl;
	// set eyes
	std::vector<float> feature;

    bool findcrop=false;
    for (auto &crop:crops) {
        if (crop.conf!=-1) {
            findcrop=true;
            break;
        }
    }
	if (findcrop) {
	    cout<<"b"<<endl;
		Crop ret_crop=crops[0];
	    cout<<"b"<<endl;
		for (auto &crop:crops) {
            if (crop.conf > ret_crop.conf) {
                ret_crop = crop;
            }
            if (crop.conf == ret_crop.conf && crop.rect.area() > ret_crop.rect.area()) {
                ret_crop = crop;
            }
        }
	    cout<<"b"<<endl;
        cv::Mat mat;
        mat=ret_crop.mat->operator()(ret_crop.rect);
	    cout<<"b"<<endl;
#ifdef debug
		face_mat=mat;
#endif
	    cout<<"b"<<endl;
		kenxnet_genFaceFeature(this->h, mat, feature);
	} else {
		//auto size=mats[0].size();
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





