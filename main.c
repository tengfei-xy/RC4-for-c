#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "time.h"
#include "bogus.h"
#include "system.h"

#define PARAM_LEN 180
#define PARAM_BDS_LEN PARAM_LEN + 3
#define ENCODE_KEY_LEN 139

char *get_dy_ABogus_main(char *param, char *user_agent);

char *b64Encode(unsigned char *string, int string_length, const char *key_table);

unsigned char *encodeWithKey(int *key, int key_len, unsigned char *data,int data_len, unsigned char *output);

void rearrangeArray(unsigned char arr[], const int indexList[]);

int main() {
    char * user_agent =  "Mozilla/5.0 (iPhone; CPU iPhone OS 13_2_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.3 Mobile/15E148 Safari/604.1";

    long start_time = time_get_timestamp_milli();

    char *param = calloc(PARAM_LEN, sizeof(char));

    for (int i = 0; i < 1000; i++) {
        snprintf(param, PARAM_LEN,
                 "_=169037328929%d&appid=1&msToken=rHfbkBM8Dh3p7BWe5FRbsUWcH2zfsukrdbU3UnTNsd0OO52hQbRbMCnJNRuNwUZGGvLRmTT0vPsD72dGcVUmXo78wDIf1fVofomNfqL1Hwdo52YhagCB",
                 i);
        char *a = get_dy_ABogus_main(param, user_agent);
//        printf("%d %lu %s\n", i,strlen(a), a);
        free(a);
    }
    free(param);

    long end_time = time_get_timestamp_milli();

    printf("平均运行时间%f毫秒\n", (end_time - start_time)/1000.0);
}

char *get_dy_ABogus_main(char *param, char *user_agent) {
    int encode_result_len = (long)strlen(user_agent);
    const char *key_table = "ckdp1h4ZKsUB80/Mfvw36XIgR25+WQAlEi7NLboqYTOPuzmFjJnryx9HVGDaStCe=";
    unsigned char *params = calloc(PARAM_BDS_LEN, sizeof(unsigned char));
    unsigned char * encode_result = calloc(encode_result_len,sizeof(unsigned char));
    unsigned char paramsArr[32] = {'\0'};
    unsigned char saltArr[32] = {'\0'};
    unsigned char tmp[BOGUS_SUM_LEN+1] = {'\0'};
    unsigned char salt[4] = "bds";
    int params_len = snprintf((char *)params, PARAM_BDS_LEN, "%sbds", param);

    bogus_s bogus;
    bogus.init = bogus_init;
    bogus.fill = bogus_fill;
    bogus.reset = bogus_reset;
    bogus.compress = bogus_compress;
    bogus.write = bogus_write;
    bogus.sum = bogus_sum;
    bogus.init(&bogus);

    // startTime = 1694077444372
    long startTime = 1694077444372;

//     paramsArr = bogus.sum(bogus.sum(params))
    bogus.sum(&bogus, bogus.sum(&bogus, params,params_len, NULL, tmp),BOGUS_SUM_LEN,NULL, paramsArr);

//    saltArr = bogus.sum(bogus.sum(salt))
    bogus.sum(&bogus,  bogus.sum(&bogus, salt,sizeof(salt)/sizeof(salt[0]) -1 , NULL, tmp), BOGUS_SUM_LEN,NULL, saltArr);


//    uaStr = b64Encode(
//            encodeWithKey([4, 1, 14], user_agent), "ckdp1h4ZKsUB80/Mfvw36XIgR25+WQAlEi7NLboqYTOPuzmFjJnryx9HVGDaStCe="
//    )
    int key_list[] = {4, 1, 14};
    encodeWithKey(key_list, sizeof(key_list) / sizeof(int), (unsigned  char *)user_agent,(int)strlen(user_agent), encode_result);

    char *uaStr = b64Encode(encode_result,encode_result_len, key_table);
    free(params);

//    uaArr = bogus.sum(uaStr)
    unsigned char uaArr[BOGUS_SUM_LEN] = {0};
    bogus.sum(&bogus, (unsigned char *) uaStr,strlen(uaStr), NULL, uaArr);
    free(uaStr);

//    timestamp1 = startTime + 20
    long timestamp1 = startTime + 20;

//    timeArr1 = []
    long timeArr1[4];

//    for x in [24, 16, 8, 0]:
//        timeArr1.append(timestamp1 >> x & 255)
    for (int i = 0, x = 24; i < 4; i++, x -= 8)
        timeArr1[i] = timestamp1 >> x & 255;

//    timestamp2 = startTime
    long timestamp2 = startTime;

//    timeArr2 = []
    long timeArr2[4];

//    for x in [24, 16, 8, 0]:
//        timeArr2.append(timestamp2 >> x & 255)
    for (long i = 0, x = 24; i < 4; i++, x -= 8)
        timeArr2[i] = timestamp2 >> x & 255;


//    totalArr = [65, *timeArr1, 0, 0, 0, 0, 4, 1, 0, 0, 0, 0, 0, 14, paramsArr[21], paramsArr[22], saltArr[21],
//            saltArr[22], uaArr[23], uaArr[24], *timeArr2, ]
    unsigned char totalArr[29];
    int s = 0;
    totalArr[s++] = 65;
    for (int i = s; s < i + 4; s++)
        totalArr[s] = (unsigned char) timeArr1[s - i];
    totalArr[s++] = 0;
    totalArr[s++] = 0;
    totalArr[s++] = 0;
    totalArr[s++] = 0;
    totalArr[s++] = 4;
    totalArr[s++] = 1;
    totalArr[s++] = 0;
    totalArr[s++] = 0;
    totalArr[s++] = 0;
    totalArr[s++] = 0;
    totalArr[s++] = 0;
    totalArr[s++] = 14;
    // 17
    totalArr[s++] = paramsArr[21];
    // 18
    totalArr[s++] = paramsArr[22];
    // 19
    totalArr[s++] = saltArr[21];
    // 20
    totalArr[s++] = saltArr[22];
    // 21
    totalArr[s++] = uaArr[23];
    // 22
    totalArr[s++] = uaArr[24];
    for (int i = s; s < i + 4; s++)
        totalArr[s] = (int) timeArr2[s - i];

//    totalArr.append(3)
    totalArr[s++] = 3;

    // totalArr.append(_tem)
    totalArr[s++] = 255;

//    totalArr = [
//    totalArr[index]
//    for index in
//    [0, 1, 5, 9, 13, 17, 19, 21, 2, 6, 10, 14, 18, 20, 22, 3, 7, 11, 15, 4, 8, 12, 16, 23, 24, 25, 26]
//    ]
    const int indexList[] = {0, 1, 5, 9, 13, 17, 19, 21, 2, 6, 10, 14, 18, 20, 22, 3, 7, 11, 15, 4, 8, 12, 16, 23, 24,
                             25, 26, 27, 28};
    rearrangeArray(totalArr, indexList);

    // _tem = 0
    int _tem = 0;

//    for x in totalArr:
//    _tem = _tem ^ x
    for (int x = 0; x < sizeof(totalArr) / sizeof(totalArr[0]); x++){
        if (totalArr[x] != 255){
            _tem = _tem ^ totalArr[x];
        }

        else
            break;

    }
    totalArr[sizeof(totalArr) / sizeof(totalArr[0]) - 1] = _tem;
//    nowTimestamp = startTime + 200
    long nowTimestamp = startTime + 200;

//    fourArr = [
//    (nowTimestamp & 255 & 170) ^ 1,
//            (nowTimestamp & 255 & 85) ^ 2,
//            (nowTimestamp >> 8 & 255 & 170) ^ 64,
//            (nowTimestamp >> 8 & 255 & 85) ^ 2,
//    ]

    // fourChr = "".join([chr(i) for i in fourArr])
    unsigned char fourArr[4];
    fourArr[0] = (unsigned char) ((nowTimestamp & 255 & 170) ^ 1);
    fourArr[1] = (unsigned char) ((nowTimestamp & 255 & 85) ^ 2);
    fourArr[2] = (unsigned char) ((nowTimestamp >> 8 & 255 & 170) ^ 64);
    fourArr[3] = (unsigned char) ((nowTimestamp >> 8 & 255 & 85) ^ 2);

//    encodeStr = encodeWithKey([131], chrStr)
    int key[1] = {131};
    const char  last_key[] = "Dkdpgh2ZmsQB80/MfvV36XI1R45-WUAlEixNLwoqYTOPuzKFjJnry79HbGcaStCe=";
    unsigned char encodeStr[30]={'\0'};

    encodeWithKey(key, sizeof(key) / sizeof(key[0]),  totalArr,sizeof(totalArr) / sizeof(totalArr[0]), encodeStr);

//    chrStr = "".join([chr(i) for i in totalArr])
    unsigned char fourChr_encodeStr[30+4] = {'\0'};
    unsigned char * chrStr = fourChr_encodeStr;
    for (int i = 0; i < 4; i++)
        fourChr_encodeStr[i] = fourArr[i];
    for (int i = 4; i < 30+4; i++)
        fourChr_encodeStr[i] = encodeStr[i - 4];

//    return b64Encode(fourChr + encodeStr, "Dkdpgh2ZmsQB80/MfvV36XI1R45-WUAlEixNLwoqYTOPuzKFjJnry79HbGcaStCe=")
    return b64Encode(chrStr,30+4-1 ,last_key);
}

// 需要释放内存
char *b64Encode(unsigned char *string, int string_length, const char *key_table) {
    int result_length = ((string_length + 2) / 3) * 4+1;
    char *result = (char *) calloc(result_length, sizeof(char));
//    int result_length = 46;
//    char result[46] = {'\0'};
    int result_index = 0;
    int i;

    for(i=0;i<string_length;i+=3){
        int num_1 = 0, num_2 = 0, num_3 = 0;
        int arr_1, arr_2, arr_3, arr_4;

        num_1 = string[i];
        num_2 = string[i + 1];
        num_3 = string[i + 2];
        if( num_2==0){
            num_3 =0;
        }

        arr_1 = num_1 >> 2;
        arr_2 = ((3 & num_1) << 4) | (num_2 >> 4);
        arr_3 = ((15 & num_2) << 2) | (num_3 >> 6);
        arr_4 = 63 & num_3;

        result[result_index++] = key_table[arr_1];
        result[result_index++] = key_table[arr_2];
        result[result_index++] = key_table[arr_3];
        result[result_index++] = key_table[arr_4];
    }
    if (i - 1 == string_length)
        result[result_length - 1] = '=';

    if (i - 2 == string_length) {
        result[result_length - 1] = '\0';
        result[result_length - 2] = '=';
    }
    return result;
}

// 输出长度为139位
unsigned char *encodeWithKey(int *key, int key_len, unsigned char *data,int data_len, unsigned char *output) {
    int result[256];
    int temp = 0;
    int temp1, temp2;
    int i;

//    for i in range(256):
//        result[i] = i
    for (i = 0; i < 256; i++) {
        result[i] = i;
    }

//    for i in range(256):
//        temp = (temp + result[i] + key[i % len(key)]) % 256
//        temp1 = result[i]
//        result[i] = result[temp]
//        result[temp] = temp1
    for (i = 0; i < 256; i++) {
        temp = (temp + result[i] + key[i % key_len]) % 256;
        temp1 = result[i];
        result[i] = result[temp];
        result[temp] = temp1;
    }


    temp2 = 0;
    temp = 0;

//    for i in range(len(data)):
//        temp2 = (temp2 + 1) % 256
//        temp = (temp + result[temp2]) % 256
//        temp1 = result[temp2]
//        result[temp2] = result[temp]
//        result[temp] = temp1
//        output += chr(ord(data[i]) ^ result[(result[temp2] + result[temp]) % 256])
    for (i = 0; i < data_len; i++) {
        temp2 = (temp2 + 1) % 256;
        temp = (temp + result[temp2]) % 256;
        temp1 = result[temp2];
        result[temp2] = result[temp];
        result[temp] = temp1;
        output[i] =data[i] ^ (result[(result[temp2] + result[temp]) % 256]);
    }
    output[i] = '\0';
    return output;
}

// 非py中的代码
// 作用:根据给定数字序列，将变量放到指定位置
void rearrangeArray(unsigned char arr[], const int indexList[]) {
    unsigned char temp[29];
    for (int i = 0; i < 29; i++) {
        temp[i] = arr[indexList[i]];
    }
    for (int i = 0; i < 29; i++) {
        arr[i] = temp[i];
    }
}