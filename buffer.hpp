
#include <opencv2/core/core.hpp>

#ifndef BUFFER_H
#define BUFFER_H

Buffer(int size);
Buffer();
~Buffer();
cv::Mat* get_buffer();
void clear_buffer();
bool is_full();
bool is_empty();
cv::Mat get_last_mat();
void set_last_mat(cv::Mat image);
int get_buffer_size();

cv::Mat* get_part_buffer(int a,int b);


#endif
