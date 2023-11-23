#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./system.h"

long xor(long x, long y) {
    x = x & 0xffffffff;
    y =  (y & 0xffffffff);
    return  x ^ y;
}

long left_shift(long x, long y) {
    x = x & 0xffffffff;
    y = y % 32;
    return x << y;
}

long unsigned_right_shift(long x, long y) {
    x = (x & 0xffffffff);
    y = y % 32;
    return x >> y;
}

long Ct(long e, long t) {
    t %= 32;
    return unsigned_right_shift(left_shift(e, t) | unsigned_right_shift(e, 32 - t), 0);
}

long St(long e) {
    if (0 <= e && e < 16)
        return 2043430169;
    else if (16 <= e && e < 64)
        return 2055708042;
    Exception("invalid j for constant Tj\n");
    return 0;
}

long kt(long e, long t, long r, long n) {
    if (0 <= e && e < 16)
        return unsigned_right_shift(xor(xor(t, r), n), 0);
    else if (16 <= e && e < 64)
        return unsigned_right_shift(t & r | t & n | r & n, 0);
    Exception("invalid j for bool function FF\n");
    return 0;
}

long xt(long e, long t, long r, long n) {
    if (0 <= e && e < 16)
        return unsigned_right_shift(xor(xor(t, r), n), 0);
    else if (16 <= e && e < 64)
        return unsigned_right_shift(t & r | ~t & n, 0);

    Exception("invalid j for bool function GG\n");
    return 0;
}

// def gt(e, t, r):
long gt(char *e, long t, long r) {
    // 唯一调用方式,但在测试中，由于调用gt代码处并没有n==hex，所以没有执行
    // c += gt(hex(self.reg[a]).replace("0x", ""), 8, "0")

    //  if len(e) >= t:
    //      return e
//     if (strlen(e) >= t)
//         return e;

    //     else:
    //         return r * (t - len(e)) + e

    return 0;
}

