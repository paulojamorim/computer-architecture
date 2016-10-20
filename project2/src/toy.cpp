/*Paulo Henrique Junqueira Amorim
  Benchmark to realize 5x5 mean filter
  
  To use: ./toy input.bmp output.png
*/

#include <iostream>
#include <fstream>
#include "CImg.h"

using namespace cimg_library;
using namespace std;
typedef unsigned char uchar;

int main(int argc, char *argv[])
{

    std::string arg;
    std::string arg_out;

    if (argc == 3)
    {
        arg = argv[1];
        arg_out = argv[2];
    }
    else
    {
        arg = "toy_input.bmp";
        arg_out = "output.png";
    }

    CImg<unsigned char> src(arg.c_str()), dest(src,false);
    
    CImg<> N(5,5);                     
    cimg_forC(src,k)                   
    cimg_for5x5(src,x,y,0,k,N,float) dest(x,y,k) = N.sum()/(5*5);   
    dest.save_png(arg_out.c_str());
    
    return 0;
}
