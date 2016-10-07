//
//g++ -Wall -g pcm-rsb.cpp -o pcm-rsb

#include <iostream>

using namespace std;

// ------ Global variables ---------------------------------------------

unsigned long long rdtsc_init;
int n_call = 1;

// -----  Assembly RDTSC function --------------------------------------

#if defined(__i386__)

static __inline__ unsigned long long rdtsc(void)
{
    unsigned long long int x;
    __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
    return x;
}

#elif defined(__x86_64__)

static __inline__ unsigned long long rdtsc(void)
{
    unsigned hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

#endif


//------- 19 "interlocked" return functions ------------------------

int func18()
{

    rdtsc_init = rdtsc();
    return 18;
}



int func17()
{
    if (n_call < 18)
        rdtsc_init = rdtsc();
    else
        func18();
    return 18;
}

int func16()
{

    if (n_call < 17)
        rdtsc_init = rdtsc();
    else
        func17();
    
    return 17;
}

int func15()
{

    if (n_call < 16)
        rdtsc_init = rdtsc();
    else
        func16();
    return 16;
}

int func14()
{

    if (n_call < 15)
        rdtsc_init = rdtsc();
    else
        func15();
    return 15;
}
int func13()
{

    if (n_call < 14)
        rdtsc_init = rdtsc();
    else
        func14();
    return 14;
}

int func12()
{

    if (n_call < 13)
        rdtsc_init = rdtsc();
    else
        func13();
    return 13;
}
int func11()
{

    if (n_call < 12)
        rdtsc_init = rdtsc();
    else
        func12();
    return 12;
}
int func10()
{

    if (n_call < 11)
        rdtsc_init = rdtsc();
    else
        func11();
    return 11;
}
int func9()
{

    if (n_call < 10)
        rdtsc_init = rdtsc();
    else
        func10();
    return 10;
}
int func8()
{

    if (n_call < 9)
        rdtsc_init = rdtsc();
    else
        func9();
    return 9;
}
int func7()
{
    
    if (n_call < 8)
        rdtsc_init = rdtsc();
    else
        func8();
    return 8;
}
int func6()
{

    if (n_call < 7)
        rdtsc_init = rdtsc();
    else
        func7();
    return 7;
}
int func5()
{

    if (n_call < 6)
        rdtsc_init = rdtsc();
    else
        func6();
    return 6;
}
int func4()
{

    if (n_call < 5)
        rdtsc_init = rdtsc();
    else
        func5();
    return 5;
}
int func3()
{

    if (n_call < 4)
        rdtsc_init = rdtsc();
    else
        func4();
    return 4;
}
int func2()
{
    if (n_call < 3)
        rdtsc_init = rdtsc();
    else
        func3();
    return 3;
}
int func1()
{

    if (n_call < 2)
        rdtsc_init = rdtsc();
    else
        func2();
    return 2;
}
int func0()
{
    if (n_call < 1)
        rdtsc_init = rdtsc();
    else
        func1();
    return 1;
}

/*--------------------------------------------------------*/


int main(int argc,  char** argv) 
{
    unsigned long long r, rdtsc_end, sum, mean;
    int current, previous, ras_value;
    int dif = 0;
    int larger_dif = 0;
    int call;
    int n_call_acum = 0;
    
    //1000x each to take mean
    for (int y=0; y < 2000; y++)
    {

        //test for each number of "interlocked" functions
        for (int value=0; value < 19; value++)
        {
            n_call = value;
            sum = 0;

            //1000x each to take mean
            for (int x=0; x<1000;x++)
            {
                //first function 
                func0();

                //end time stamp 
                rdtsc_end = rdtsc();     
       
                //difference (end - init)
                r = rdtsc_end - rdtsc_init;
                sum += r;
            }
        
            mean = sum/1000;
            current = mean;
           
            //to take difference 
            if (value == 0)
            {
                previous = mean;
            }
            else
            {
                dif = current - previous;
                current = mean;
                previous = current;
            }


            //to take largest difference to each interaction
            if (dif > larger_dif)
            {
                larger_dif = dif;
                call = value + 1;
            }
           
        }

        //mean of all largest difference
        n_call_acum += call;

    } 
   
    //mean 
    ras_value = n_call_acum/2000;

    cout << "\nO RAS de seu processador possuí aproximadamente " << ras_value << " posições.\n\n";
    
    return 0 ;
}
