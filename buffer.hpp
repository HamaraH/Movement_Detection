
#include <opencv2/opencv.hpp>

#ifndef BUFFER_H
#define BUFFER_H

Mat* get_buffer(){};
void clear_buffer(){};
boolean is_full(){};
Mat get_last_mat(){};
void set_last_mat(Mat image){};
int get_buffer_size(){};
Buffer(int size){};
Buffer(){};
Mat* get_part_buffer(int a,b);
boolean is_empty(){};

#endif
