
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
    int head;

  public:

     Buffer(int size);
     Buffer();
     ~Buffer();

    int getSize();
    int getHead();

    cv::Mat* getBuffer();

     void clearBuffer();

     bool isFull();
     bool isEmpty();

     cv::Mat getLastMat();
     void addMat(cv::Mat image);
};

#endif
