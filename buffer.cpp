#include <opencv2/opencv.hpp>
//#include <buffer.hpp>

class Buffer{

private:

  Mat buffer[];   //Stockage des images
  int size;       //Taille du tableau
  int last_index; //Index de la dernière image stockée

public:

  Buffer(int size=30){  //Constructeur

    this.buffer = (Mat*) malloc(size*sizeof(Mat));

  }


  Mat* get_buffer(){    //Obtenir le buffer en entier

    return this.buffer;

  };

  void clear_buffer(){  //Remettre le buffer à 0

    for ( int i = 0 ; i < this.size ; i++ ){

      this.buffer[i] = NULL;

    }

  }

  boolean is_full(){  //Verifier si le buffer est rempli

    if (this.buffer[ size-1 ] == NULL)

        return  true;

    else

      return false;

    }

  }

  Mat get_mat(){

    return buffer[last_index-1];

  }

  void set_mat(Mat image){

    this.buffer[last_index] = image;

  }


}
