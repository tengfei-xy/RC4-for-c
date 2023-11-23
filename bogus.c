#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "bit.h"
#include "bogus.h"
#include "system.h"
#include "strings.h"

#define min(x, y) x > y ? y : x

void bogus_init(bogus_s *b) {

    b->size = 0;
    b->chunk_size = 0;

    b->chunk = calloc(b->chunk_size + 1, sizeof(char));
    for (int i = 0; i < b->chunk_size + 1; i++)
        b->chunk[i] = INIT_FLAG;
}

void bogus_over(bogus_s *b) {
    free(b->chunk);
}

// 将chunk补充到64的整数倍
void bogus_fill(bogus_s *b) {
    // o = 8 * self.size
    long o = 8 * b->size;

    // self.chunk.append(128)
    b->chunk_size += 1;
    b->chunk = (unsigned char *) realloc(b->chunk, b->chunk_size);
    b->chunk[b->chunk_size - 1] = 128;

    // i = len(self.chunk) % 64
    // if 64 - i < 8:
    //     i -= 64
    int i = b->chunk_size % 64;
    int o_chunk_size = b->chunk_size;
    b->chunk_size = b->chunk_size % 64 > 56 ? b->chunk_size + 8 : b->chunk_size;
    b->chunk_size = ((b->chunk_size / 64) + 1) * 64;  // 扩大A到64的倍数
    b->chunk = (unsigned char *) realloc(b->chunk, b->chunk_size);
//    b->chunk[b->chunk_size-1]=0;

    // while (i < 56)
    //     self.chunk.append(0)
    //     i += 1
    for (int j = o_chunk_size; j < b->chunk_size - 8; j++) {
        b->chunk[j] = '\0';
    }

    // for a in range(4):
    //     c = math.floor(o / 4294967296)
    //     self.chunk.append(unsigned_right_shift(c, 8 * (3 - a)) & 255)
    for (int a = b->chunk_size - 8; a < b->chunk_size - 4; a++) {
        long c = floor((double) o / (double) 4294967296);
        b->chunk[a] = unsigned_right_shift(c, 8 * (3 - a) & 255);
    }

    // for a in range(4):
    //     self.chunk.append(unsigned_right_shift(o, 8 * (3 - a)) & 255)
    for (int a = b->chunk_size - 4; a < b->chunk_size; a++) {
        // o在发生变化
        b->chunk[a] = unsigned_right_shift(o, 8 * (3 - a) & 255);
    }


}

void bogus_reset(bogus_s *b) {
    // self.reg = [1937774191, 1226093241, 388252375, 3666478592, 2842636476, 372324522, 3817729613, 2969243214]
    b->reg[0] = 1937774191;
    b->reg[1] = 1226093241;
    b->reg[2] = 388252375;
    b->reg[3] = 3666478592;
    b->reg[4] = 2842636476;
    b->reg[5] = 372324522;
    b->reg[6] = 3817729613;
    b->reg[7] = 2969243214;


    // self.chunk = []
    memset(b->chunk, 0, b->chunk_size);

    memset(b->reg_i, 0, BOGUS_REG_LEN);
    memset(b->summary, 0, BOGUS_SUM_LEN);
    memset(b->window, 0, BOGUS_CHUNK_LEN);

    b->chunk_size = 0;

    // self.size = 0
    b->size = 0;

    b->chunk = realloc(b->chunk, b->chunk_size + 1);
    for (int i = 0; i < b->chunk_size + 1; i++)
        b->chunk[i] = INIT_FLAG;
}

void bogus_write(bogus_s *b, unsigned char *e, int e_len) {
    // if isinstance(e, str):
    //     o = [ord(i) for i in list(e)]
    // else:
    //     o = e
    int o_len = e_len;
    unsigned char *o = calloc(o_len, sizeof(char));
    memcpy(o, e, o_len);

    // self.size += len(o)
    b->size += o_len;

    // i = 64 - len(self.chunk)
    int i = 64 - b->chunk_size;

    // if len(o) < i:
    if (o_len < i) {
        //  self.chunk.extend(o)
        b->chunk_size = b->chunk_size == 0 ? o_len : b->chunk_size + o_len;
        b->chunk = realloc(b->chunk, b->chunk_size);
        if (!b->chunk)
            Exception("memory allocation failure");
        memcpy(b->chunk, o, b->chunk_size);
        free(o);
        return;
    }

    // self.chunk.extend(o[:64])
    b->chunk_size = b->chunk_size == 0 ? 64 : b->chunk_size + 64;
    b->chunk = realloc(b->chunk, b->chunk_size);
    if (!b->chunk)
        Exception("memory allocation failure");
    memcpy(b->chunk, o, 64);

    // while len(self.chunk) >= 64:
    while (b->chunk_size >= 64) {
        // self._compress(self.chunk)
        b->compress(b, b->chunk);

        // if i < len(o):
        if (i < o_len) {

            // self.chunk = o[i: min(i + 64, len(o))]
            int m = min(i + 64, o_len);
            int copy_len = m - i;
            memcpy(b->chunk, o + i, copy_len);
            b->chunk[copy_len] = '\0';
            b->chunk_size = copy_len;

        } else

            // self.chunk = []
            memset(b->chunk, 0, b->chunk_size);
        i += 64;
    }

    free(o);
}

// 影响的是reg
void bogus_compress(bogus_s *b, unsigned char *r) {

//    if len(r) < 64:
//        raise Exception("compress error: not enough data")
//    if (strlen(r) < 64)
//        Exception("compress error: not enough data");

    // t = [0] * 132
    long t[132] = {0};

    // o = anonymous(r)
    long *o = bogus_compress_anonymous(b, t, r);
    if (!o)
        Exception("memory allocation failure");

    // i = self.reg[0:]
    // 应该动态分配，但总是遇到分配错误的问题
    // malloc: Heap corruption detected, free list is damaged
    // 因此换成静态分配方法
//    long *i = (long *)malloc(BOGUS_REG_LEN* sizeof(long));
    long *i = b->reg_i;
    memcpy(i, b->reg, sizeof(b->reg));

//    for a in range(64):
//        c = Ct(i[0], 12) + i[4] + Ct(St(a), a)
//        c = 4294967295 & c
//        c = Ct(unsigned_right_shift(c, 0), 7)
//        f = unsigned_right_shift(xor(c, Ct(i[0], 12)), 0)
//        u = kt(a, i[0], i[1], i[2])
//        u = u + i[3] + f + o[a + 68]
//        u = unsigned_right_shift(4294967295 & u, 0)
//        s = xt(a, i[4], i[5], i[6])
//        s = s + i[7] + c + o[a]
//        s = unsigned_right_shift(4294967295 & s, 0)
//        i[3] = i[2]
//        i[2] = Ct(i[1], 9)
//        i[1] = i[0]
//        i[0] = u
//        i[7] = i[6]
//        i[6] = Ct(i[5], 19)
//        i[5] = i[4]
//        i[4] = unsigned_right_shift(xor(xor(s, Ct(s, 9)), Ct(s, 17)), 0)
    for (int a = 0; a < 64; a++) {
        long c = Ct(i[0], 12) + i[4] + Ct(St(a), a);
        c = 4294967295 & c;
        c = Ct(unsigned_right_shift(c, 0), 7);
        long f = unsigned_right_shift(xor(c, Ct(i[0], 12)), 0);
        long u = kt(a, i[0], i[1], i[2]);
        u = u + i[3] + f + o[a + 68];
        u = unsigned_right_shift(4294967295 & u, 0);
        long s = xt(a, i[4], i[5], i[6]);
        s = s + i[7] + c + o[a];
        s = unsigned_right_shift(4294967295 & s, 0);
        i[3] = i[2];
        i[2] = Ct(i[1], 9);
        i[1] = i[0];
        i[0] = u;
        i[7] = i[6];
        i[6] = Ct(i[5], 19);
        i[5] = i[4];
        i[4] = unsigned_right_shift(xor(xor(s, Ct(s, 9)), Ct(s, 17)), 0);
    }

//    for l in range(8):
//        self.reg[l] = unsigned_right_shift(xor(self.reg[l], i[l]), 0)
    for (int l = 0; l < 8; l++)
        b->reg[l] = unsigned_right_shift(xor(b->reg[l], i[l]), 0);

//    free(o);

}

long *bogus_compress_anonymous(bogus_s *b, long *t, unsigned char *e) {


//    for r in range(16):
//    t[r] = left_shift(e[4 * r], 24)
//    t[r] |= left_shift(e[4 * r + 1], 16)
//    t[r] |= left_shift(e[4 * r + 2], 8)
//    t[r] |= e[4 * r + 3]
//    t[r] = unsigned_right_shift(t[r], 0)
    for (int r = 0; r < 16; r++) {
        t[r] = left_shift(e[4 * r], 24);
        t[r] |= left_shift(e[4 * r + 1], 16);
        t[r] |= left_shift(e[4 * r + 2], 8);
        t[r] |= e[4 * r + 3];
        t[r] = unsigned_right_shift(t[r], 0);
    }

//    for n in range(16, 68):
//        o = xor(xor(t[n - 16], t[n - 9]), Ct(t[n - 3], 15))
//        o = xor(xor(o, Ct(o, 15)), Ct(o, 23))
//        t[n] = unsigned_right_shift(xor(xor(o, Ct(t[n - 13], 7)), t[n - 6]), 0)
    for (int n = 16; n < 68; n++) {
        long o = xor(xor(t[n - 16], t[n - 9]), Ct(t[n - 3], 15));
        o = xor(xor(o, Ct(o, 15)), Ct(o, 23));
        t[n] = unsigned_right_shift(xor(xor(o, Ct(t[n - 13], 7)), t[n - 6]), 0);
    }

//    for n in range(64):
//        t[n + 68] = unsigned_right_shift(xor(t[n], t[n + 4]), 0)
    for (int n = 0; n < 64; n++)
        t[n + 68] = unsigned_right_shift(xor(t[n], t[n + 4]), 0);

    return t;
}

unsigned char *bogus_sum(bogus_s *b, unsigned char *r, int r_len, char *n, unsigned char *result) {


    b->reset(b);
    b->write(b, r, r_len);
    b->fill(b);

//    for a in range(0, len(self.chunk), 64):
//        win = self.chunk[a: a + 64]
    for (int i = 0; i < b->chunk_size; i += 64)
        b->compress(b, memcpy(b->window, b->chunk + i, 64));

//    if n == "hex":
//        c = ""
    unsigned char *c = b->summary;
    if (string_compare(n, "hex")) {

//        for a in range(8):
//            c += gt(hex(self.reg[a]).replace("0x", ""), 8, "0")

        //  self.reset()
        b->reset(b);

        return c;
    }
//    else:
//        c = [0] * 32
//        for a in range(8):
//        f = self.reg[a]
//        c[4 * a + 3] = unsigned_right_shift(255 & f, 0)
//        f = unsigned_right_shift(f, 8)
//        c[4 * a + 2] = unsigned_right_shift(255 & f, 0)
//        f = unsigned_right_shift(f, 8)
//        c[4 * a + 1] = unsigned_right_shift(255 & f, 0)
//        f = unsigned_right_shift(f, 8)
//        c[4 * a] = unsigned_right_shift(255 & f, 0)
    for (int a = 0; a < 8; a++) {
        long f = b->reg[a];
        c[4 * a + 3] = unsigned_right_shift((long) (255 & f), 0);
        f = unsigned_right_shift(f, 8);
        c[4 * a + 2] = unsigned_right_shift(255 & f, 0);
        f = unsigned_right_shift(f, 8);
        c[4 * a + 1] = unsigned_right_shift(255 & f, 0);
        f = unsigned_right_shift(f, 8);
        c[4 * a] = unsigned_right_shift(255 & f, 0);
    }

    for (int i = 0; i < BOGUS_SUM_LEN; i++)
        if (c[i] >= 0 && c[i] <= 255)
            result[i] = (unsigned char) c[i];

    //  self.reset()
//    b->reset(b);
    return result;
}