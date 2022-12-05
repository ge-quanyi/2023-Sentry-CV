//
// Created by quonone on 22-11-23.
//

#ifndef WEBVIDEO_CAMERA_H
#define WEBVIDEO_CAMERA_H
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

class Camera{
public:
    Camera(const int cam_index, const int img_width, int img_height, float expose_time):
    index(cam_index),image_width(img_width),image_height(img_height),expose_time(expose_time)
    {}
    ~Camera(){
        delete cam_handle;
    }
    bool init(){
        cam_handle = new cv::VideoCapture(index);
        if(!cam_handle->isOpened())
        {
            std::cout<<"[ Error ]: open camera "<<index<<" failed! The program will exit ..."<<std::endl;
            delete cam_handle;
            exit(-1);
        }
        cam_handle->set(cv::CAP_PROP_FRAME_WIDTH, image_width);
        cam_handle->set(cv::CAP_PROP_FRAME_HEIGHT,image_height);
        cam_handle->set(cv::CAP_PROP_EXPOSURE, expose_time);
        return true;
    }

    void update(cv::Mat& frame){
        cam_handle->read(frame);
    }
    void release(){
        delete cam_handle;
    }
private:
    int ret, index;
    const int image_width, image_height;
    float expose_time;
    cv::VideoCapture *cam_handle;
};

#endif //WEBVIDEO_CAMERA_H
