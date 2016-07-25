//
//  File.h
//  EncryptData
//
//  Created by JunAFa on 15-1-17.
//  Copyright (c) 2015年 no2game. All rights reserved.
//

#ifndef __EncryptData__File__
#define __EncryptData__File__

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <limits.h>
#include <list>



struct File_Struct{
    std::string mFileName;
    std::string mPathStr;
    File_Struct(std::string& fileName,std::string& pathStr)
    {
        mFileName = fileName;
        mPathStr = pathStr;
    }
    
};

struct FileInfo
{
    std::string name;
    int crc;
    int size;
};

class No2File {
public:
    static int	PathExist(char *strPath);
    static int	CreateMultDir(const char *dir);
    
    
    //遍历文件夹函数
    static void TraverseFolder(const std::string& sourthPath,const std::string& subpath,std::vector<File_Struct>& pathMap,
                               std::vector<std::string>& fileFliterVec,std::vector<std::string>& pathFliterVec);
    
    static void getFiles(const std::string& rootpath,const std::string& subpath, std::list<FileInfo* > & filelist );
};


#endif /* defined(__EncryptData__File__) */
