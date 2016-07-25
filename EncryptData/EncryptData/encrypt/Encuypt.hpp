//
//  Encuypt.hpp
//  EncryptData
//
//  Created by JunAFa on 15-1-17.
//  Copyright (c) 2015年 no2game. All rights reserved.
//

#ifndef EncryptData_Encuypt_hpp
#define EncryptData_Encuypt_hpp

#include <iostream>
#include <string>
#include <assert.h>
#include "rc4.h"
#include "No2DncryptData.h"
#include<sys/types.h>
#include<sys/stat.h>
#include "zlib.h"

#include "File.h"

using namespace std;
#define CC_BREAK_IF(cond) if(cond) break



static unsigned char* _getFileData(const char* pszFileName, const char* pszMode, unsigned long* pSize)
{
    unsigned char* pBuffer = NULL;
    
    *pSize = 0;
    do
    {
        // read the file from hardware
        FILE *fp = fopen(pszFileName, pszMode);
        CC_BREAK_IF(!fp);
        
        fseek(fp,0,SEEK_END);
        *pSize = ftell(fp);
        fseek(fp,0,SEEK_SET);
        pBuffer = new unsigned char[*pSize];
        *pSize = fread(pBuffer,sizeof(unsigned char), *pSize,fp);
        fclose(fp);
    } while (0);
    
    return pBuffer;
}

//////////////////////////////////////////////////////////////////////////
static bool changeElToLuaAndSave(const std::string& filePath, const std::string& destPath)
{
    unsigned long filesize = 0;
    unsigned char* filebuf = _getFileData(filePath.c_str(), "rb", &filesize);
    
    if(filebuf)
    {
        unsigned long outBufferSize = 0;
        ssize_t value = outBufferSize;
        unsigned char* outBuffer = 0;
        if(decBuffer(filesize,filebuf,value,outBuffer))
        {
            FILE* fp = fopen(destPath.c_str(), "wb"); //
            size_t return_size = fwrite(outBuffer, 1, outBufferSize, fp);
            fclose(fp);
            
            delete[] outBuffer;
            delete[] filebuf;
            
            return true;
        }
        delete[] outBuffer;
        delete[] filebuf;
    }
    
    return false;
}

static bool changeLuaToElAndSave(const std::string& filePath, const std::string& destPathDir, FileInfo* pInfo, std::string& destPath,std::string& newfilepath)
{
    
    unsigned long filesize = 0;
    unsigned char* filebuf = _getFileData(filePath.c_str(), "rb", &filesize);
    
    if(filebuf)
    {
        unsigned long outBufferSize = 0;
        unsigned char* outBuffer = 0;
        ssize_t value = filesize;
        if(encBuffer(filesize,filebuf,value,outBuffer))
        {
            std::string::size_type pos1 = filePath.find_last_of("/");
            std::string::size_type pos2 = filePath.find_last_of(".");
            
            newfilepath = destPathDir +
            filePath.substr(pos1, pos2-pos1) + ".el";
            
            FILE* fp = fopen(newfilepath.c_str(), "wb"); //
            size_t return_size = fwrite(outBuffer, 1, outBufferSize, fp);
            fclose(fp);
            
            
            {
                unsigned short crcvalue = 0;
                
                char szTemp[512] = {0};
                strcpy(szTemp, pInfo->name.c_str());
                int iLen = pInfo->name.length();
                szTemp[iLen-1] = 0;
                szTemp[iLen-2] = 'l';
                szTemp[iLen-3] = 'e';
                
                pInfo->name = szTemp;
                pInfo->crc = crcvalue;
                pInfo->size = outBufferSize;
            }
            
            delete[] outBuffer;
            delete[] filebuf;
            
            destPath = newfilepath;
            return true;
        }
        delete[] outBuffer;
        delete[] filebuf;
    }
    
    
    return false;
}

static bool decryptTxtAndSave(const std::string& filePath, const std::string& destPath)
{
    unsigned long size = 0;
    unsigned char* dataIn = _getFileData(filePath.c_str(), "rb", &size);
    
    if (dataIn)
    {
        {
            unsigned long outBufferSize = 0;
            unsigned char* outBuffer = 0;
            ssize_t value = size;
            ssize_t value2 = outBufferSize;
            if(decBuffer(value-2,dataIn+2,value2,outBuffer))//
            {
                FILE* fp = fopen(destPath.c_str(), "wb"); //
                size_t return_size = fwrite(outBuffer, 1, outBufferSize, fp);
                fclose(fp);
                
                delete[] outBuffer;
                delete[] dataIn;
                
                return true;
            }
            delete[] outBuffer;
            delete[] dataIn;
        }
    }
    
    return false;
}

static bool encryptTxtAndSave(const std::string& filePath, const std::string& destPathDir, FileInfo* pInfo, std::string& destPath)
{
    
    unsigned long size = 0;
    unsigned char* dataIn = _getFileData(filePath.c_str(), "rb", &size);
    
    if (dataIn)
    {
        std::string::size_type pos1 = filePath.find_last_of("/");
        std::string::size_type pos2 = filePath.find_last_of(".");
        
        std::string newfilepath = destPathDir +
        filePath.substr(pos1);
        
        {
            unsigned long outBufferSize = 0;
            unsigned char* outBuffer = 0;
            ssize_t value1 = size;
            ssize_t value2 = outBufferSize;
            if(encBuffer(value1,dataIn,value2,outBuffer))//先压缩再加密
            {
                FILE* fp = fopen(newfilepath.c_str(), "wb"); //
                
                if(!fp)
                {
                    return false;
                }
                unsigned char head[2];
                head[0] = 0xef;
                head[1] = 0xfe;
                
                fwrite(head, 1, 2, fp);
                
                size_t return_size = fwrite(outBuffer, 1, outBufferSize, fp);
                fclose(fp);
                
                {
                    {
                        unsigned char* szTemp = new unsigned char[outBufferSize+5];
                        szTemp[0] = 0xef;
                        szTemp[1] = 0xfe;
                        szTemp[outBufferSize+2] = 0;
                        szTemp[outBufferSize+3] = 0;
                        szTemp[outBufferSize+4] = 0;
                        memcpy(szTemp+2, outBuffer, outBufferSize);
                        
                        unsigned short crcvalue = 0;
                        
                        pInfo->crc = crcvalue;
                        pInfo->size = outBufferSize+2;
                        
                        delete[] szTemp;
                    }
                }
                
                delete[] outBuffer;
                delete[] dataIn;
                
                destPath = newfilepath;
                return true;
            }
            delete[] outBuffer;
            delete[] dataIn;
        }
    }
    
    return false;
}

/************************************************************************/
/* 加密图片文件                                                                     */
/************************************************************************/
static bool encryptImageAndSave(std::string sourceFilePath,const std::string& destPathDir,std::string& destPath, FileInfo* pInfo)
{
    unsigned char* dataOut=NULL;
    unsigned long sourceBufferSize = 0;
    const int pngEncodeSize = 128;
    const int dataBufferSize = 1024*1024;
    unsigned char* codeBuffer = _getFileData(sourceFilePath.c_str(),"rb",&sourceBufferSize);
    unsigned long writeBufferSize = 0;
    AVRC4 rc4_key;
    memset(&rc4_key,0,sizeof(AVRC4));
    av_rc4_init(&rc4_key,gamekey,strlen((char*)gamekey)*8);
    int markSize = 3;
    dataOut = new unsigned char[sourceBufferSize+markSize];
    dataOut[0] = 0xef;
    dataOut[1] = 0xfe;
    dataOut[2] = 0x80;
    int bufferSize = 0;
    int encodeSize = 0x80;
    (sourceBufferSize > encodeSize) ? bufferSize = encodeSize : bufferSize = sourceBufferSize;
    av_rc4_crypt(&rc4_key,dataOut + markSize,codeBuffer,bufferSize);
    if (bufferSize < sourceBufferSize)
    {
        memcpy(dataOut + encodeSize + markSize,codeBuffer+encodeSize,sourceBufferSize - encodeSize);
    }
    writeBufferSize = sourceBufferSize+markSize;
    std::cout << "Encrypt file : " << sourceFilePath << " done." << std::endl;
    
    std::string newfilepath = destPathDir +destPath;
    FILE *g_fpLog;
    int spilt = newfilepath.find_last_of("/");
    No2File::CreateMultDir(newfilepath.substr(0, spilt).c_str());
    g_fpLog = fopen(newfilepath.c_str(), "wb");
    assert(g_fpLog);
    fwrite(dataOut,1,writeBufferSize,g_fpLog);
    unsigned short crcvalue = 0;
    pInfo->crc = crcvalue;
    pInfo->size = writeBufferSize;
    fclose(g_fpLog);
    delete[] dataOut;
    delete[] codeBuffer;
    
    return true;
}

/************************************************************************/
/* 解密图片文件                                                                     */
/************************************************************************/
static bool decodeRC4TextureBuffer(const std::string& filePath, const std::string& destPath)
{
    unsigned long inSize = 0;
    unsigned char* inBuffer = _getFileData(filePath.c_str(), "rb", &inSize);
    
    if (inBuffer)
    {
        unsigned char* outBuffer = NULL;
        ssize_t value = inSize;
        inBuffer=rc4TextureBuffer(value,inBuffer,&value);
        FILE* fp = fopen(destPath.c_str(), "wb"); //
        if(fp)
        {
            size_t return_size = fwrite(inBuffer, 1, inSize, fp);
        }
        fclose(fp);
        return true;
    }
    
    return false;
}

/************************************************************************/
/* 加密plist和ccbi文件                                                                     */
/************************************************************************/
static bool encryptRC4DocumentAndSave(std::string sourceFilePath,const std::string& destPathDir,std::string& destPath, FileInfo* pInfo)
{
    unsigned char* dataOut=NULL;
    unsigned long sourceBufferSize = 0;
    const int pngEncodeSize = 128;
    const int dataBufferSize = 1024*1024;
    unsigned char* codeBuffer = _getFileData(sourceFilePath.c_str(),"rb",&sourceBufferSize);
    unsigned long writeBufferSize = 0;
    AVRC4 rc4_key;
    memset(&rc4_key,0,sizeof(AVRC4));
    av_rc4_init(&rc4_key,gamekey,strlen((char*)gamekey)*8);
    int markSize = 3;
    unsigned long compressSize = (unsigned long)(sourceBufferSize * 1.5);
    unsigned char* compressData = new unsigned char[compressSize];
    int ret = compress(compressData,&compressSize,codeBuffer,sourceBufferSize);
    if (ret == Z_OK)
    {
        dataOut = new unsigned char[compressSize+3];
        dataOut[0] = 0xef;
        dataOut[1] = 0xfe;
        dataOut[2] = 0xff;
        av_rc4_crypt(&rc4_key,dataOut+3,compressData,compressSize);
        writeBufferSize = compressSize+3;
        
        delete[] compressData;
    }
    else
    {
        delete[] dataOut;
        delete[] codeBuffer;
        delete[] compressData;
        std::cout << "Encrypt file Failed : " << sourceFilePath << " done." << std::endl;
        return false;
    }
    
    
    
    std::string newfilepath = destPathDir + destPath;
    
    if(sourceFilePath.find(".lua")!=std::string::npos)
    {
        int spilt = destPath.find_last_of(".");
        newfilepath = destPathDir + destPath.substr(spilt) + ".el";
        char szTemp[512] = {0};
        strcpy(szTemp, pInfo->name.c_str());
        int iLen = pInfo->name.length();
        szTemp[iLen-1] = 0;
        szTemp[iLen-2] = 'l';
        szTemp[iLen-3] = 'e';
        pInfo->name = szTemp;
    }
    
    destPath=newfilepath;
    FILE *g_fpLog;
    int spilt = newfilepath.find_last_of("/");
    No2File::CreateMultDir(newfilepath.substr(0, spilt).c_str());
    g_fpLog = fopen(newfilepath.c_str(), "wb");
    assert(g_fpLog);
    fwrite(dataOut,1,writeBufferSize,g_fpLog);
    int value = writeBufferSize;
    unsigned short crcvalue = 0;
    pInfo->crc = crcvalue;
    pInfo->size = 0;
    fclose(g_fpLog);
    delete[] dataOut;
    delete[] codeBuffer;
    std::cout << "Encrypt file : " << sourceFilePath << " done." << std::endl;
    return true;
}

/************************************************************************/
/* 解密plist和ccbi文件                                                  */
/************************************************************************/
static bool decodeRC4DocumentBuffer(const std::string& filePath, const std::string& destPath)
{
    unsigned long inSize = 0;
    unsigned char* inBuffer = _getFileData(filePath.c_str(), "rb", &inSize);
    
    if (inBuffer)
    {
        unsigned char* outBuffer = NULL;
        ssize_t value = inSize;
        inBuffer=rc4DocumentBuffer(value,inBuffer,&value);
        FILE* fp = fopen(destPath.c_str(), "wb"); //
        if(fp)
        {
            if(inBuffer)
            {
                size_t return_size = fwrite(inBuffer, 1, inSize, fp);  
            }
        }
        fclose(fp);
        return true;
    }
    
    return false;
}

#endif
