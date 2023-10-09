//
// Created by 24546 on 2023/9/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>

FILE  *destFp;
int test() {
    char *destFile = "d://2/test.h264";

    destFp = fopen(destFile, "wb");
    if (destFp == NULL) {
        printf("destFp open error!");
        return 0;
    }
    return 1;
}

int writeFile(char *buf,int length){
    fwrite(buf, 1, length, destFp);
}

int closeFile(){
    fclose(destFp);
    return 1;
}