#include <stdio.h>
#include <math.h>
#include "_float.h"

int main()
{
    float f_0_15625 = 0.15625f;
    float f_pi = 3.14231231231;
    char buff[120];
    _ftoa(buff, f_0_15625, 6, 120);
    printf("p=6: %s\n", buff);
    _ftoa(buff, -f_0_15625, 3, 120);
    printf("p=3: %s\n", buff);
    _ftoa(buff, -f_0_15625, 8, 120);
    printf("p=8: %s\n", buff);
    _ftoa(buff, f_pi, 8, 120);
    printf("p=8: %s\n", buff);
    _ftoa(buff, f_pi, 0, 120);
    printf("p=0: %s\n", buff);
    _ftoa(buff, f_pi-3.0, 0, 120);
    printf("p=0: %s\n", buff);
    _ftoa(buff, f_pi-3.0, 1, 120);
    printf("p=1: %s\n", buff);
    _ftoa(buff, f_pi-3.0, 2, 120);
    printf("p=2: %s\n", buff);
    _ftoa(buff, 12.2, 1, 120);
    printf("p=1: %s\n", buff);
    _ftoa(buff, 9.2, 1, 120);
    printf("p=1: %s\n", buff);
    _ftoa(buff, 10.0, 1, 120);
    printf("p=1: %s\n", buff);
    _ftoa(buff, 0.003, 1, 120);
    printf("p=1: %s\n", buff);
    _ftoa(buff, 0.003, 2, 120);
    printf("p=2: %s\n", buff);
    _ftoa(buff, 0.003, 3, 120);
    printf("p=3: %s\n", buff);
    _ftoa(buff, 0.003, 4, 120);
    printf("p=4: %s\n", buff);


    _ftoa(buff, NAN, 1, 120);
    printf("buff nan: %s\n", buff);

    _ftoa(buff, 1.0f/0.0f, 1, 120);
    printf("buff +inf: %s\n", buff);
    _ftoa(buff, -1.0f/0.0f, 1, 120);
    printf("buff -inf: %s\n", buff);


    static const char *nums[] = { "2.324", "101", "-32.24", "  3.14111", NULL};
    for (int i=0; nums[i] != NULL; i++){
        printf("atof(\"%s\"): %f\n", nums[i], _atof(nums[i]));
    }

}
