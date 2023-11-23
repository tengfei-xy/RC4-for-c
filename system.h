
#define INIT_FLAG -1
// 作用: 返回毫秒时间戳
long time_get_timestamp_milli();

void Exception(char * err);
void output_x(unsigned char *chunk, int chunk_len);
void output_reg(long * reg);
void output_r(unsigned char * r,int r_len);
void output_sum(char * value,unsigned char *str);
void output_res(char * value,unsigned char *str,int str_len);