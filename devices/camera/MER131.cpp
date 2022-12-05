//
// Created by Quanyi on 2022/12/3.
//
#include <chrono>
#include <string>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <thread>
#include "camera_api.h"
#include "umt/umt.hpp"
#include "MER131.h"
#define img_width 960
#define img_height 768

std::chrono::time_point<std::chrono::steady_clock> start_time;
bool if_time_set = false;
//umt::Publisher<cameraData> data_pub("camera_data");

/**
 * 回调采集
 * @param pFrame
 */
//void imgTransform(GX_FRAME_CALLBACK_PARAM *pFrame){
//    if(pFrame->status == GX_FRAME_STATUS_SUCCESS){
//        auto img_timestamp = std::chrono::steady_clock::now();
//        if(!if_time_set){
//            start_time = img_timestamp;
//            if_time_set = true;
//        }
//        char* origin_buff = new char[img_width * img_height * 3];
//        memset(origin_buff, 0, pFrame->nWidth * pFrame->nHeight * 3 * sizeof(char ));
//        DX_BAYER_CONVERT_TYPE cvtype = RAW2RGB_NEIGHBOUR;           //选择插值算法
//        DX_PIXEL_COLOR_FILTER nBayerType = BAYERBG;                   //选择图像Bayer格式
//        VxInt32 DxStatus = DxRaw8toRGB24(const_cast<void *>(pFrame->pImgBuf), origin_buff, pFrame->nWidth,
//                                         pFrame->nHeight, cvtype, nBayerType, false);
//        if(DxStatus != DX_OK){
//            delete[] origin_buff;
//            return ;
//        }
//        cv::Mat image_rgb24(pFrame->nHeight, pFrame->nWidth, CV_8UC3);
//        memcpy(image_rgb24.data, origin_buff, pFrame->nHeight * pFrame->nWidth * 3);
//        ///TODO process origin image
//        double time_stamp = std::chrono::duration<double>(img_timestamp-start_time).count();
//
//        data_pub.push({time_stamp, image_rgb24});
//        delete[] origin_buff;
//    }
//}

void camera_task_run(){
    GX_STATUS status = Config();
    if (status != GX_STATUS_SUCCESS) {
        std::cout << "config Camera Faile ..." << std::endl;
        return ;
    }
    camera_config cam0_info;
    cam0_info.sn_str = "KE0200120159";
    cam0_info.SN = &cam0_info.sn_str[0];
    MercureDriver *cam0 = new MercureDriver(cam0_info);
    cam0->InitCamera();
    if (cam0->status != GX_STATUS_SUCCESS) {
        std::cout << "Initial Camera Faile ..." << std::endl;
        return;
    }
    /*回调采集*/
//    status = GXRegisterCaptureCallback(cam0->hDevice_, NULL, imgTransform);
//    status = GXSendCommand(cam0->hDevice_, GX_COMMAND_ACQUISITION_START);
//    if (status != GX_STATUS_SUCCESS) {
//        std::cout << "Cam0 Start Read Faile ..." << std::endl;
//        return ;
//    }

//    while(true){
//        std::this_thread::sleep_for(std::chrono::milliseconds(50));//50ms
//    }
    PGX_FRAME_BUFFER pFrameBuffer;
    status = GXStreamOn(cam0->hDevice_);
    if(status != GX_STATUS_SUCCESS)
        return ;
    umt::Publisher<cameraData> data_pub("camera_data");
    while(true){

        status = GXDQBuf(cam0->hDevice_, &pFrameBuffer, 1000);
        if(status == GX_STATUS_SUCCESS){
            auto t0 = std::chrono::steady_clock::now();
            if(pFrameBuffer->nStatus == GX_FRAME_STATUS_SUCCESS){
                auto img_timestamp = std::chrono::steady_clock::now();
                if(!if_time_set){
                    start_time = img_timestamp;
                    if_time_set = true;
                }
                char* origin_buff = new char[img_width * img_height * 3];
                //memset(origin_buff, 0, pFrameBuffer->nWidth * pFrameBuffer->nHeight * 3 * sizeof(char ));
                DX_BAYER_CONVERT_TYPE cvtype = RAW2RGB_NEIGHBOUR;           //选择插值算法
                DX_PIXEL_COLOR_FILTER nBayerType = BAYERBG;                   //选择图像Bayer格式
                VxInt32 DxStatus = DxRaw8toRGB24(const_cast<void *>(pFrameBuffer->pImgBuf), origin_buff, pFrameBuffer->nWidth,
                                                 pFrameBuffer->nHeight, cvtype, nBayerType, false);
                if(DxStatus != DX_OK){
                    delete[] origin_buff;
                    return ;
                }
                cv::Mat image_rgb24(pFrameBuffer->nHeight, pFrameBuffer->nWidth, CV_8UC3);
                memcpy(image_rgb24.data, origin_buff, pFrameBuffer->nHeight * pFrameBuffer->nWidth * 3);
                ///TODO process origin image
                double time_stamp = std::chrono::duration<double>(img_timestamp-start_time).count();

                data_pub.push({time_stamp, image_rgb24});
                delete[] origin_buff;
            }
            status = GXQBuf(cam0->hDevice_, pFrameBuffer);
            auto t1 = std::chrono::steady_clock::now();
            double per = std::chrono::duration<double>(t1-t0).count();
            //std::cout<<"[info] cost time "<<per*1000<<" ms"<<std::endl;
        }
    }

    status = GXStreamOff(cam0->hDevice_);
    cam0->StopCamera();

}

void camera_thread(){
    std::thread(camera_task_run).detach();
}
PYBIND11_EMBEDDED_MODULE(Camera, m){
    m.def("camera_task_run", camera_thread);
}