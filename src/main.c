#include <stdio.h>
#include "pico/stdlib.h"
#include "app.h"


int main(void)
{
    stdio_init_all();
    
    while(true) {
        app_main();
    }
    return 0;
}

