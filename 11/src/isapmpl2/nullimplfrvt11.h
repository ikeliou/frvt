/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility  whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef NULLIMPLFRVT11_H_
#define NULLIMPLFRVT11_H_

#include "frvt11.h"

#include <dlib/dnn.h>
#include <dlib/clustering.h>
#include <dlib/string.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/matrix.h>
using namespace dlib;
template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual = add_prev1<block<N,BN,1,tag1<SUBNET>>>;

template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual_down = add_prev2<avg_pool<2,2,2,2,skip1<tag2<block<N,BN,2,tag1<SUBNET>>>>>>;

template <int N, template <typename> class BN, int stride, typename SUBNET> 
using block  = BN<con<N,3,3,1,1,relu<BN<con<N,3,3,stride,stride,SUBNET>>>>>;

template <int N, typename SUBNET> using ares      = relu<residual<block,N,affine,SUBNET>>;
template <int N, typename SUBNET> using ares_down = relu<residual_down<block,N,affine,SUBNET>>;

template <typename SUBNET> using alevel0 = ares_down<256,SUBNET>;
template <typename SUBNET> using alevel1 = ares<256,ares<256,ares_down<256,SUBNET>>>;
template <typename SUBNET> using alevel2 = ares<128,ares<128,ares_down<128,SUBNET>>>;
template <typename SUBNET> using alevel3 = ares<64,ares<64,ares<64,ares_down<64,SUBNET>>>>;
template <typename SUBNET> using alevel4 = ares<32,ares<32,ares<32,SUBNET>>>;

using anet_type = loss_metric<fc_no_bias<128,avg_pool_everything<
                            alevel0<
                            alevel1<
                            alevel2<
                            alevel3<
                            alevel4<
                            max_pool<3,3,2,2,relu<affine<con<32,7,7,2,2,
                            input_rgb_image_sized<150>
                            >>>>>>>>>>>>;
/*
 * Declare the implementation class of the FRVT 1:1 Interface
 */
namespace FRVT_11 {
    class NullImplFRVT11 : public FRVT_11::Interface {
public:

    NullImplFRVT11();
    ~NullImplFRVT11() override;

    FRVT::ReturnStatus
    initialize(const std::string &configDir) override;

    FRVT::ReturnStatus
    createTemplate(
            const FRVT::Multiface &faces,
            FRVT::TemplateRole role,
            std::vector<uint8_t> &templ,
            std::vector<FRVT::EyePair> &eyeCoordinates) override;

    FRVT::ReturnStatus
    matchTemplates(
            const std::vector<uint8_t> &verifTemplate,
            const std::vector<uint8_t> &enrollTemplate,
            double &similarity) override;

    static std::shared_ptr<FRVT_11::Interface>
    getImplementation();

private:
    std::string configDir;
    static const int featureVectorSize{4};
    // Some other members
	shape_predictor sp;
	anet_type net;
};
}

#endif /* NULLIMPLFRVT11_H_ */
