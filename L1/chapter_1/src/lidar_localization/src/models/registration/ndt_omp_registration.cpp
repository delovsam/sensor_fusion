/*
 * @Description: NDTOMP 匹配模块
 * @Author: Ren Qian
 * @Date: 2020-02-08 21:46:45
 */
#include <lidar_localization/tools/tic_toc.h>
#include "lidar_localization/models/registration/ndt_omp_registration.hpp"

#include "glog/logging.h"

namespace lidar_localization {

NDTOMPRegistration::NDTOMPRegistration(const YAML::Node& node)
    :ndt_ptr_(new pclomp::NormalDistributionsTransform<CloudData::POINT, CloudData::POINT>()) {
    
    float res = node["res"].as<float>();
    float step_size = node["step_size"].as<float>();
    float trans_eps = node["trans_eps"].as<float>();
    int max_iter = node["max_iter"].as<int>();
    int num_threads = node["num_threads"].as<int>();
    SetRegistrationParam(res, step_size, trans_eps, max_iter, num_threads);
}

NDTOMPRegistration::NDTOMPRegistration(float res, float step_size, float trans_eps, int max_iter, int num_threads)
    :ndt_ptr_(new pclomp::NormalDistributionsTransform<CloudData::POINT, CloudData::POINT>()) {

    SetRegistrationParam(res, step_size, trans_eps, max_iter, num_threads);
}

bool NDTOMPRegistration::SetRegistrationParam(float res, float step_size, float trans_eps, int max_iter, int num_threads) {
    ndt_ptr_->setResolution(res);
    ndt_ptr_->setStepSize(step_size);
    ndt_ptr_->setTransformationEpsilon(trans_eps);
    ndt_ptr_->setMaximumIterations(max_iter);
    ndt_ptr_->setNumThreads(num_threads);
    ndt_ptr_->setNeighborhoodSearchMethod(pclomp::KDTREE);
    LOG(INFO) << "NDTOMP 的匹配参数为：" << std::endl
              << "res: " << res << ", "
              << "step_size: " << step_size << ", "
              << "trans_eps: " << trans_eps << ", "
              << "max_iter: " << max_iter  << ", "
              << "num_threads: " << num_threads
              << std::endl << std::endl;

    return true;
}

bool NDTOMPRegistration::SetInputTarget(const CloudData::CLOUD_PTR& input_target) {
    ndt_ptr_->setInputTarget(input_target);

    return true;
}

bool NDTOMPRegistration::ScanMatch(const CloudData::CLOUD_PTR& input_source, 
                                const Eigen::Matrix4f& predict_pose, 
                                CloudData::CLOUD_PTR& result_cloud_ptr,
                                Eigen::Matrix4f& result_pose) {
    td::TicToc timer;
    ndt_ptr_->setInputSource(input_source);
    ndt_ptr_->align(*result_cloud_ptr, predict_pose);
    result_pose = ndt_ptr_->getFinalTransformation();
    align_time += timer.tos();
    align_count += 1;
    align_mean_time();
    return true;
}
}