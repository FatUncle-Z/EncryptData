// encryption_aes.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <iostream>
#include <bitset>
#include <utility>
#include <string>
#include <fstream>
#include <iostream>

#include "No2AES.h"
#include "GameMaths.h"
#include "Encuypt.hpp"
#include "File.h"

using namespace std;

void createEncryptFile(const std::string& inputDir,const std::string& outputDir)
{

    std::list<FileInfo* >  filelist;
    No2File::getFiles(inputDir,"",filelist);
    
    
    std::list<FileInfo*>::iterator it = filelist.begin();
    for(;it!=filelist.end();++it)
    {
        int extpos = (*it)->name.find_last_of('.');
        
        if(extpos!=-1)
        {
            std::string oriExt = (*it)->name.substr(extpos,(*it)->name.length());
            transform(oriExt.begin(), oriExt.end(), oriExt.begin(), (int (*)(int))tolower);
            
            
            
            std::string destPath = outputDir + (*it)->name;
             std::string destPath1 = (*it)->name;
            std::string destDir = outputDir;
            std::string::size_type pos = destPath.find_last_of("/");
            

            //--begin
            if (oriExt == ".txt" || oriExt == ".cfg" || oriExt==".json" )
            {//处理txt文件，加密
                bool bret = encryptTxtAndSave(inputDir + (*it)->name, destDir, (*it), destPath1);
                if (bret)
                {
                    std::cout<<"encrypt file = "<<(*it)->name<<" is Scuessed !"<<std::endl;
                }
                else
                {
                    std::string strCmd = "echo failed: \"" + inputDir + (*it)->name + "\"";
                    std::cout<<strCmd<<std::endl;
                }
            }
            else if(oriExt==".png" || oriExt == ".jpg")
            {//处理图片资源加密
               
                bool bRet=encryptImageAndSave(inputDir + (*it)->name,destDir,destPath1,(*it));
                if(bRet)
                {
                    std::cout<<"encrypt file = "<<(*it)->name<<" is Scuessed !"<<std::endl;
                }
                else
                {
                    std::string strCmd = "echo failed: \"" + inputDir + (*it)->name + "\"";
                    std::cout<<strCmd<<std::endl;
                }
            }
            else if(oriExt==".ccbi" || oriExt==".plist")
            {//处理ccbi文件加密
                std::string tempName=(*it) ->name;
                bool bRet=encryptRC4DocumentAndSave(inputDir + (*it)->name,destDir,destPath1,(*it));
                if (bRet)
                {
                    std::cout<<"encrypt file = "<<(*it)->name<<" is Scuessed !"<<std::endl;
                }
                else
                {
                    std::string strCmd = "echo failed: \"" + inputDir + (*it)->name + "\"";
                    std::cout<<strCmd<<std::endl;
                }
            }
        }
    }
}



int main(int argc, char* argv[])
{
    std::string inputDir  = "/Volumes/development/cocosproject/DevProj/RunMan/RunMan/Publish";
    std::string outputDir = "/Volumes/development/cocosproject/DevProj/RunMan/RunMan/a/";
    createEncryptFile(inputDir, outputDir);
	return 0;
}

