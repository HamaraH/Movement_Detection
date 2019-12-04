
#include <opencv2/core/core.hpp>

#ifndef BUFFER_H
#define BUFFER_H

cv::Mat* get_buffer();
void clear_buffer();
boolean is_full();
cv::Mat get_last_mat();
void set_last_mat(cv::Mat image);
int get_buffer_size();
Buffer(int size);
Buffer();
cv::Mat* get_part_buffer(int a,b);
boolean is_empty();

#endif
