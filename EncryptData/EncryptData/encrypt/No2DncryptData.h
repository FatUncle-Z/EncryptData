//
//  No2DncryptData.h
//  RunMan
//
//  Created by JunAFa on 15-1-17.
//
//

#ifndef __RunMan__No2DncryptData__
#define __RunMan__No2DncryptData__

#include <string>

const unsigned char gamekey[]="nc315XHW^*/./$2QWE1325G";
const int blocksize = 1024*8;
const int DATA_BUFFER_SIZE = 1024*1024;
const int MARK_SIZE = 3;

bool encBuffer(ssize_t inSize , unsigned char* inBuffer, ssize_t& outSize, unsigned char*& outBuffer);

bool decBuffer(ssize_t inSize , unsigned char* inBuffer, ssize_t& outSize, unsigned char*& outBuffer);

unsigned char* rc4DocumentBuffer(ssize_t inSize, unsigned char* inBuffer, ssize_t* outSize);

unsigned char* rc4TextureBuffer(ssize_t inSize, unsigned char* inBuffer, ssize_t* outSize);

unsigned char* getDncodeBuffer( ssize_t* inSize, unsigned char* buffer, const char * fileName,const char* pszMode);

#endif /* defined(__RunMan__No2DncryptData__) */
