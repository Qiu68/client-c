//
// Created by 24546 on 2023/9/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>

FILE  *destFp;
int writeFileInit() {
    char *destFile = "d://2/test.h264";

    destFp = fopen(destFile, "wb");
    if (destFp == NULL) {
        printf("destFp open error!");
        return 0;
    }
    return 1;
}

int writer(long long offset,char *buf,int length){
    long long seek = fseek(destFp,offset,0);
    int count = fwrite(buf, 1, length, destFp);
//    printf("------  length = %d  count = %lld ------\n",length,offset);
//    fflush(stdout);
    fflush(destFp);
    return count;
}

int closeFile(){
    fclose(destFp);
    return 1;
}