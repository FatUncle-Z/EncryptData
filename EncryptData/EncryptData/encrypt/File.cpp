//
//  File.cpp
//  EncryptData
//
//  Created by JunAFa on 15-1-17.
//  Copyright (c) 2015年 no2game. All rights reserved.
//

#include "File.h"

#include <iostream>
#include <string>

#include "GameMaths.h"
#include "Encuypt.hpp"

#define   MAX_PATH   200 

int  No2File::PathExist(char *strPath)
{
    return (access(strPath, 0) == 0);
}
int No2File::CreateMultDir(const char *path)
{
    char DirName[256];
    strcpy(DirName, path);
    int i, len = strlen(DirName);
    if(DirName[len-1]!='/')
        strcat(DirName,   "/");
    
    len   =   strlen(DirName);
    
    for(i=1;   i<len;   i++)
    {
        if(DirName[i]=='/')
        {
            DirName[i]   =   0;
            if(   access(DirName,   NULL)!=0   )
            {
                if(mkdir(DirName,   0755)==-1)
                {
                    perror("mkdir   error");
                    return   -1;
                }
            }
            DirName[i]   =   '/';
        }
    }

    return 1;
}

//遍历文件夹函数
void No2File::TraverseFolder(const std::string& sourthPath,const std::string& subpath,std::vector<File_Struct>& pathMap,
                    std::vector<std::string>& fileFliterVec,std::vector<std::string>& pathFliterVec)
{
    std::string temp = sourthPath;
    temp.append("/");
    temp.append(subpath);
    temp.append("/*.*");
    const char* rootpath = temp.c_str();
    
    DIR* dir;
    dirent* ptr;
    struct stat stStatBuf;
    char* p = nullptr;
    if (chdir(rootpath) == -1)
    {
        printf( "Your   Inputed   Argument   %s   is   Not   Valid\n ",rootpath);
        exit(1);
    }
    
    dir = opendir(rootpath);
    while ((ptr = readdir(dir)) != NULL)
    {
        if   (stat(ptr-> d_name,   &stStatBuf)   ==   -1)
        {
            printf( "Get   the   stat   error   on   file:%s\n ",   ptr-> d_name);
            continue; 
        }
        
        if((stStatBuf.st_mode & S_IFDIR) && strcmp(ptr-> d_name, ". ")!= 0
           &&   strcmp(ptr-> d_name, ".. ") != 0)
        {
            std::string fileName = ptr->d_name;
            bool isFliterPath = false;
            std::vector<std::string>::iterator iter = pathFliterVec.begin();
            for (; iter!= pathFliterVec.end();++iter)
            {
                if (*iter == fileName)
                {
                    isFliterPath = true;
                    break;
                }
            }
            if (!isFliterPath)
            {
                std::string thesubpath = subpath+"/"+ptr->d_name;
                TraverseFolder(sourthPath,thesubpath,pathMap,fileFliterVec,pathFliterVec);
            }
        }
        else
        {
            std::string fileName = ptr->d_name;
            bool isContain = false;
            std::string::size_type dotPos = fileName.find_last_of(".");
            std::string suffixStr;
            if (dotPos == std::string::npos)
            {
                suffixStr = "";
            }
            else
            {
                suffixStr = fileName.substr(dotPos,fileName.length());
            }
            std::vector<std::string>::iterator iter = fileFliterVec.begin();
            if (fileFliterVec.size()!=0)
            {
                for (; iter!= fileFliterVec.end(); ++iter)
                {
                    if (suffixStr == *iter)
                    {
                        isContain = true;
                        break;
                    }
                }
            }
            if (isContain)
            {
                std::string thefilePath = subpath+"/";
                thefilePath.append(fileName);
                
                File_Struct fileStr(fileName,thefilePath);
                pathMap.push_back(fileStr);
            }
        }
        chdir(rootpath);
    }
    
    closedir(dir);
}

void No2File::getFiles(const std::string& rootpath,const std::string& subpath, std::list<FileInfo* > & filelist )
{
    std::string temp = rootpath;
//    temp.append("/");
    temp.append(subpath);
//    temp.append("/*.*");
    const char* root = temp.c_str();

    DIR* dir;
    dirent* ptr;
    struct stat stStatBuf;
    char* p = nullptr;
    std::cout<<temp<<std::endl;
    if (chdir(root) == -1)
    {
        printf( "Your   Inputed   Argument   %s   is   Not   Valid\n ",root);
        exit(1);
    }
    dir = opendir(root);
    while ((ptr = readdir(dir)) != NULL)
    {
        if   (stat(ptr-> d_name,   &stStatBuf)   ==   -1)
        {
            printf( "Get   the   stat   error   on   file:%s\n ",   ptr-> d_name);
            continue;
        }
        
        if((stStatBuf.st_mode & S_IFDIR) && strcmp(ptr-> d_name, ".")!= 0
           &&   strcmp(ptr-> d_name,   "..") != 0)
        {
            std::string thesubpath = subpath+"/"+ptr-> d_name;
            getFiles(rootpath,thesubpath,filelist);
        }
        else
        {
            if (strcmp(ptr-> d_name,".svn") == 0)
                continue;
            
            std::string thefile = subpath+"/";
            thefile.append(ptr-> d_name);
            unsigned long size = 0;
            unsigned char* dataIn = _getFileData((rootpath+"/"+thefile).c_str(),"rb",&size);
            unsigned short crcvalue = GameMaths::GetCRC16(dataIn,size);
            FileInfo *fileinfo = new FileInfo;
            fileinfo->name = thefile;
            fileinfo->crc = crcvalue;
            fileinfo->size = size;
            filelist.push_back(fileinfo);
            delete[] dataIn;
        }
        chdir(root);
    }
    closedir(dir);
}