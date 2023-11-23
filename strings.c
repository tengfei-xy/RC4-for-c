#include "strings.h"
#include <iconv.h>
#include <string.h>
#include <ctype.h>

bool string_compare(char *left, char *right)
{
    if (left == NULL || right == NULL)
        return false;
    char *a = left, *b = right;

    int len_a = (int)strlen(a);
    int len_b = (int)strlen(b);

    if (len_a != len_b)
        return false;
    if (a[0] != b[0])
        return false;
    if (len_a > 1)
    {
        if (a[1] != b[1])
            return false;
    }

    if (a[len_a - 1] != b[len_b - 1])
        return false;

    for (int i = 0; i < len_b; i++)
        if (left[i] != right[i])
            return false;
    return true;
}