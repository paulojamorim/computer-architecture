#include <iostream>
#include <fstream>
#include "CImg.h"

using namespace cimg_library;
using namespace std;
typedef unsigned char uchar;

int main(int argc, char *argv[])
{

    std::string arg;
    if (argc > 1)
    {
        arg = argv[2];
    }
    else
    {
        arg = "toy_input.bmp";  
    }

    CImg<unsigned char> src(arg.c_str()), dest(src,false);
    
    CImg<> N(5,5);                     
    cimg_forC(src,k)                   
    cimg_for5x5(src,x,y,0,k,N,float) dest(x,y,k) = N.sum()/(5*5);   
    CImgList<unsigned char> view(src,dest);
    view.display("Paulo Amorim - Toy Benchmark for MO601 (Original + Filtered)"); 

    return 0;
}


