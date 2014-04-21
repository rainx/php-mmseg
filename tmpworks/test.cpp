#include <stdio.h>
#include <string.h>
#include <fstream>
#include <string>
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <map>

#include "SegmenterManager.h"
#include "Segmenter.h"
#include "csr_utils.h"

using namespace css;

int main()
{
    const char content[] = "你好世界, 我是徐景, 你是谁？\0";
    SegmenterManager* mgr = new SegmenterManager();
    int nRet = 0;
    nRet = mgr->init("/opt/mmseg/etc");
    Segmenter* seg = mgr->getSegmenter();
    u4 content_length = strlen(content);

    seg->setBuffer((u1*)content, content_length);

    while(1)
    {
        char tword[100];
        memset(tword, 0, 100);
        u2 len = 0, symlen = 0;
        char* tok = (char*)seg->peekToken(len,symlen);
        if(!tok || !*tok || !len){
            break;
        }

        strncpy(tword, tok, len);
        //append new item
        printf("%s , %d\n", tword, len);
        seg->popToken(len);
    }

    if (mgr) {
        delete mgr;
    }

    return 0;
}
