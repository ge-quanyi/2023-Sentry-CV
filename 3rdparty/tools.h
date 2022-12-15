//
// Created by quonone on 22-12-15.
//

#ifndef ARESCV_TOOLS_H
#define ARESCV_TOOLS_H

#include <iostream>
#include <chrono>

namespace tool{

    class Timer{
    public:
        Timer(){};
        ~Timer(){};
        void get_time(){
            this_time = std::chrono::steady_clock::now();
        }

        double interval(){
            double period =  std::chrono::duration<double>(this_time - last_time).count(); //ms
            last_time = this_time;
            return period;
        }

    private:
        std::chrono::time_point<std::chrono::steady_clock> this_time;
        std::chrono::time_point<std::chrono::steady_clock> last_time;
    };













}
#endif //ARESCV_TOOLS_H
