#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>

#include "./system.h"
void Exception(char *err)
{
    printf("%s\n", err);
    exit(-1);
}
long time_get_timestamp_milli()
{
    struct timeb st = {0};
    ftime(&st);
    time_t t = time(NULL);
    return time(&t) * 1000 + st.millitm;
}
void output_x(unsigned char *chunk, int chunk_len)
{
    printf("chunk(%d)=",chunk_len);
    for (int i = 0; i < chunk_len; i++)
        printf("%d, ", chunk[i]);
    putchar('\n');
    fflush(stdout);
}
void output_sum(char * value,unsigned char *str)
{
    printf("%s=",value);
    for (int i = 0; i < 32; i++)
        printf("%d, ", str[i]);
    putchar('\n');
    fflush(stdout);
}
void output_reg(long * reg)
{
    printf("reg ");
    for (int i = 0; i < 8; i++)
        printf("%ld, ", reg[i]);
    putchar('\n');
    fflush(stdout);
}
void output_r(unsigned char * r,int r_len)
{
    printf("r(%d)=",r_len);
    for (int i = 0; i < r_len; i++)
        printf("%d, ", r[i]);
    putchar('\n');
    fflush(stdout);
}
void output_res(char * value,unsigned char *str,int str_len)
{
    printf("%s(%d)=",value,str_len);
    for (int i = 0; i < str_len; i++)
        printf("%d, ", str[i]);
    putchar('\n');
    fflush(stdout);
}