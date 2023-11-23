#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <openssl/md5.h>
// 作用: 字符串对比
// 注意: 大小写敏感
bool string_compare(char *left, char *right);