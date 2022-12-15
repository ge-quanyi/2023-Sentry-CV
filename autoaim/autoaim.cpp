//
// Created by Quanyi on 2022/12/3.
//

#include <umt/umt.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <pybind11/numpy.h>
#include <thread>
#include "autoaim.h"
#include "../devices/serial/serial.h"
#include "camera/MER131.h"

extern SerialPort port;
void autoaim(){ //auto aim run

    umt::Subscriber<cameraData> camera_sub("camera_data");
    umt::Publisher<cv::Mat> pub("debug_img");

    while (true){
        cameraData* data = new cameraData;
        try{
            //cv::waitKey(3);
            *data = camera_sub.pop();
            cv::Mat raw_img = data->img;
            //std::cout<<"img size "<<raw_img.cols<<std::endl;
            double time_stamp = data->timestamp;
            putText(raw_img, "testing font",cv::Point2f(0,30),cv::FONT_HERSHEY_COMPLEX_SMALL,1, cv::Scalar(255,0,0));
            pub.push(raw_img);

        }catch (...){
            std::cout << "[WARNING] camera not ready." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        delete data;
//        char* send_data = new char[6];
//        send_data[0] = int16_t (1000*pitch);
//        send_data[1] = int16_t (1000*pitch) >> 8;
//        send_data[2] = int16_t (1000*yaw);
//        send_data[3] = int16_t (1000*yaw) >> 8;
//        send_data[4] = int16_t (1000*dis);
//        send_data[5] = int16_t (1000*dis) >> 8;
//        port.SendBuff(cmd, send_data, 6);
//        delete[] send_data;
       // std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
/**
 *
 * no use
 */
//void Subscriber(){
//    umt::Subscriber<cv::Mat> sub("debug_img", 1);
//
//    cv::Mat img;
//    while(true){
//        try {
//            img = sub.pop();
//            std::cout<< " [info] img size : "<< img.rows <<std::endl;
//        } catch(...) {
//            std::cout << "[WARNING] pub_A not ready." << std::endl;
//            std::this_thread::sleep_for(std::chrono::milliseconds(200));
//        }
//    }
//}

void background_task_run(){
    std::thread(autoaim).detach();
}

namespace py = pybind11;
py::array_t<uint8_t> cvMat2npArray(const cv::Mat &mat) {
    py::array_t<uint8_t> array({mat.rows, mat.cols, mat.channels()});
    cv::Mat ref_mat(mat.rows, mat.cols, CV_8UC(mat.channels()), array.mutable_data());
    mat.copyTo(ref_mat);
    return array;
}

// numpy.ndarray --> cv::Mat
cv::Mat npArray2cvMat(const py::array_t<uint8_t> &array) {
    cv::Mat mat;
    return mat;
}

UMT_EXPORT_MESSAGE_ALIAS(cvMat, cv::Mat, c) {

    c.def(py::init<cv::Mat>());
    c.def(py::init(&npArray2cvMat));
    c.def("get_nparray", cvMat2npArray);
}

PYBIND11_EMBEDDED_MODULE(Autoaim, m){
    namespace py = pybind11;
    m.def("background_task_run", background_task_run);
}