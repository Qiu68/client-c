//
// Created by 24546 on 2023/9/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>

#define BUFFER_SIZE  64 * 1024
#define pt printf

int test() {
    char *srcFile = "d://test1.h264";
    char *destFile = "d://2/test.h264";

    FILE *srcFp, *destFp;

    srcFp = fopen(srcFile, "rb");
    if (srcFp == NULL) {
        printf("srcFp open error!");
        return 0;
    }

    destFp = fopen(destFile, "wb");
    if (destFp == NULL) {
        printf("destFp open error!");
        return 0;
    }

    struct stat st = {0};
    stat(srcFile, &st);
    int buffSize = st.st_size;

    if (BUFFER_SIZE > buffSize) {
        buffSize = BUFFER_SIZE;
    }

    char *buffer = malloc(buffSize);

    while (!feof(srcFp)) {
        int length = fread(buffer, 1, buffSize, srcFp);
        fwrite(buffer, 1, length, destFp);
    }

    free(buffer);
    fclose(srcFp);
    fclose(destFp);
    return 1;
}