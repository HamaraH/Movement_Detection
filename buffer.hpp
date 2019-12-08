
#include <opencv2/core/core.hpp>

#ifndef BUFFER_H
#define BUFFER_H

class Buffer {

  private:
    //Stockage des images
    cv::Mat* buffer; 
    
    //Taille du tableau
    int size;
    
    //Index de la dernière image stockée
    int last_index; 
    
  public:
  
     Buffer(int size);
     Buffer();
     ~Buffer();
     
    int get_buffer_size(); 
    int get_last_index();

    cv::Mat* get_buffer();
     
     void clear_buffer();
     
     bool is_full();
     bool is_empty();
     
     cv::Mat get_last_mat();
     void set_last_mat(cv::Mat image);
};

#endif

