#include<light_object.h>
#include<stdio.h>

#ifdef PICO_RP2040
#   include <pico/stdio.h>
#endif
//#include <pico/stdlib.h>

int main(int argc, char *argv[])
{
#ifdef PICO_RP2040
    stdio_init_all();
#endif

    
}
