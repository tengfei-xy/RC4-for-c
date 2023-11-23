#include <stdbool.h>

#define BOGUS_CHUNK_LEN 64
#define BOGUS_SUM_LEN 32
#define BOGUS_REG_LEN 8
typedef struct bogus_s
{

    long reg[BOGUS_REG_LEN];
    long reg_i[BOGUS_REG_LEN];
    unsigned char * chunk;
    unsigned char window[BOGUS_CHUNK_LEN];
    unsigned char summary[BOGUS_SUM_LEN];

    int chunk_size;
    int size;

    void (*init)(struct bogus_s *);
    void (*fill)(struct bogus_s *);
    void (*reset)(struct bogus_s *);
    void (*compress)(struct bogus_s *, unsigned char *r);
    void (*write)(struct bogus_s *,unsigned char *, int);
    unsigned char * (*sum)(struct bogus_s *, unsigned char *,int , char * ,unsigned char *);
} bogus_s;
void bogus_init(bogus_s *b);
void bogus_fill(bogus_s *b);
void bogus_reset(bogus_s *b);
void bogus_compress(bogus_s *b,unsigned char * r);
long * bogus_compress_anonymous(bogus_s *b,long*t,unsigned char * e);
void bogus_write(bogus_s *b, unsigned char *e, int e_len);
unsigned char * bogus_sum(bogus_s *b, unsigned char * r,int r_len, char *n,unsigned char * result);
