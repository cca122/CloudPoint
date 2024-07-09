#pragma once
#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
using std::ifstream,std::string;

namespace zhywytDataLoder{
    enum TYPE{
        CSV,
        TXT,
        PNG
    };
    std::map<int,string>tailMap={
        {CSV,".scv"},
        {TXT,".txt"}
    };
    bool fileIsExist(string fileName);
    std::vector<std::filesystem::path> allFileOfDir(string dir,int type);
    std::vector<std::filesystem::path> allDirOfDir(string dir);
    // functionDef
    class DataLoder;
    // —————————————————————copy right————————————————————————————
    #ifdef _WIN32
    #include <direct.h>		//for mkdir rmdir
    #include <io.h>			//for access
    #elif __linux__
    #include <unistd.h>		//for mkdir rmdir
    #include <sys/stat.h>	//for access
    #include <dirent.h>		//for DIR remove
    #endif
    
    #ifdef _WIN32
    #define ACCESS _access
    #define MKDIR(a) _mkdir((a))
    #define RMDIR(a) _rmdir((a))
    #elif __linux__
    #define ACCESS access
    #define MKDIR(a) mkdir((a),0755)
    #define RMDIR(a) rmdir((a))
    #endif

    bool MkDir(const std::string& strPath)
    {
        int i = 0;
        int nDirLen = strPath.length();
        if (nDirLen <= 0)
            return false;
        char *pDirTemp = new char[nDirLen + 4];
        strPath.copy(pDirTemp, nDirLen + 1, 0);// +1 to copy '\0'
        pDirTemp[nDirLen] = '\0';
        //在末尾加'/'
        if (pDirTemp[nDirLen - 1] != '\\' && pDirTemp[nDirLen - 1] != '/')
        {
            pDirTemp[nDirLen] = '/';
            pDirTemp[nDirLen + 1] = '\0';
            nDirLen++;
        }
        // 创建目录
        for (i = 0; i < nDirLen; i++)
        {
            if (pDirTemp[i] == '\\' || pDirTemp[i] == '/')
            {
                pDirTemp[i] = '\0';//截断后面的子目录，逐级查看目录是否存在，若不存在则创建
                //如果不存在,创建
                int statu;
                statu = ACCESS(pDirTemp, 0);
                if (statu != 0)//可能存在同名文件导致没有创建
                {
                    statu = MKDIR(pDirTemp);
                    if (statu != 0)//可能上级不是文件夹而是同名文件导致创建失败
                    {
                        return false;
                    }
                }
                //支持linux,将所有\换成/
                pDirTemp[i] = '/';
            }
        }
        delete[] pDirTemp;
        return true;
    }
    bool RmDir(const std::string & path)
    {
        std::string strPath = path;
    #ifdef _WIN32
        struct _finddata_t fb;   //查找相同属性文件的存储结构体
        //制作用于正则化路径
        if (strPath.at(strPath.length() - 1) != '\\' || strPath.at(strPath.length() - 1) != '/')
            strPath.append("\\");
        std::string findPath = strPath + "*";
        intptr_t handle;//用long类型会报错
        handle = _findfirst(findPath.c_str(), &fb);
        //找到第一个匹配的文件
        if (handle != -1L)
        {
            std::string pathTemp;
            do//循环找到的文件 
            {
                //系统有个系统文件，名为“..”和“.”,对它不做处理  
                if (strcmp(fb.name, "..")!=0 && strcmp(fb.name, ".")!=0)//对系统隐藏文件的处理标记
                {
                    //制作完整路径
                    pathTemp.clear();
                    pathTemp = strPath + std::string(fb.name);
                    //属性值为16，则说明是文件夹，迭代  
                    if (fb.attrib == _A_SUBDIR)//_A_SUBDIR=16
                    {
                        RmDir(pathTemp.c_str());
                    }
                    //非文件夹的文件，直接删除。对文件属性值的情况没做详细调查，可能还有其他情况。  
                    else
                    {
                        remove(pathTemp.c_str());
                    }
                }
            } while (0 == _findnext(handle, &fb));//判断放前面会失去第一个搜索的结果
            //关闭文件夹，只有关闭了才能删除。找这个函数找了很久，标准c中用的是closedir  
            //经验介绍：一般产生Handle的函数执行后，都要进行关闭的动作。  
            _findclose(handle);
        }
        //移除文件夹  
        return RMDIR(strPath.c_str())==0?true:false;
    
    #elif __linux__
        if (strPath.at(strPath.length() - 1) != '\\' || strPath.at(strPath.length() - 1) != '/')
            strPath.append("/");
        DIR *d = opendir(strPath.c_str());//打开这个目录
        if (d != NULL)
        { 
            struct dirent *dt = NULL;
            while (dt = readdir(d))//逐个读取目录中的文件到dt
            {
                //系统有个系统文件，名为“..”和“.”,对它不做处理
                if (strcmp(dt->d_name, "..")!=0 && strcmp(dt->d_name, ".")!=0)//判断是否为系统隐藏文件
                {
                    struct stat st;//文件的信息
                    std::string fileName;//文件夹中的文件名
                    fileName = strPath + std::string(dt->d_name);
                    stat(fileName.c_str(), &st);
                    if (S_ISDIR(st.st_mode))
                    {
                        RmDir(fileName);
                    }
                    else
                    {
                        remove(fileName.c_str());
                    }
                }
            }
            closedir(d);
        }
        return rmdir(strPath.c_str())==0?true:false;
    #endif
    
    }
    // 版权声明：本文为博主原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接和本声明。
    // 原文链接：https://blog.csdn.net/venom_snake/article/details/88066475
    bool fileIsExist(string fileName){
        // 0 /* Test for existence.  */
        return access(fileName.c_str(),F_OK)==0;
    }
    // type is file tail like .txt .csv
    std::vector<std::filesystem::path> allFileOfDir(string dir,int type){
        if(!fileIsExist(dir)){
            throw(dir +"is not exits!");
        }
        std::vector<std::filesystem::path> files;
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            if (entry.is_regular_file() && entry.path().extension() == tailMap[type]) {
                files.push_back(entry.path());
            }
        }
        return files;
    }
    std::vector<std::filesystem::path> allDirOfDir(string dir){
        if(!fileIsExist(dir)){
            throw(dir +"is not exits!");
        }
        std::vector<std::filesystem::path> files;
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            if (std::filesystem::is_directory(entry)) {
                files.push_back(entry.path());
            }
        }
        return files;

    }
    /*
    每个DataLoder读入一个物体，并生成对应的数据结构，数据结构设计

    */
    class DataLoder{
    private:
        class DataPng{
            int height,width;
            string name;
        public:
            std::vector<std::vector<float>> data;
            std::filesystem::path path;
            void Cast(){//Cast the 
                for(auto& ii:data){
                    for(auto& i:ii){
                        i = i/ 65535.0 * 3000.0; 
                    }
                }
            }
            DataPng(std::filesystem::path FilePath):path(FilePath){
                cv::Mat depthImage = cv::imread(FilePath,cv::IMREAD_UNCHANGED);     //cv::IMREAD_UNCHANGED确保不改变图像的原始格式
                if(depthImage.empty()){
                    throw(string("Can't open file")+FilePath.string());
                }
                if (depthImage.type() != CV_16U && depthImage.type() != CV_32F) {
                    throw("Depth image is not in a supported format.");
                }
                height = depthImage.rows;
                width = depthImage.cols;
                data.reserve(height*width);
                const float* src = depthImage.ptr<float>(0);
                for (int i = 0; i < height; i++){
                    for (int j = 0; j < width; j++){
                        data[i][j] = src[i * width + j];
                    }
                }
                Cast();
            }
        };
        // use artworkxxx or botxxx as title
        // filename, data
        std::map<string,DataPng>depths;
        std::map<string,Eigen::Matrix<double,3,3> >Cams;
        std::map<string,Eigen::Matrix<double,3,4> >RTs;
        string FileRoot;
        string FilePre;
    public:
        // is one artwork | bottle with many picture
        DataLoder(const string& fileRoot, const string& filePre):FilePre(filePre),FileRoot(fileRoot){
            string DepthPath = FileRoot+FilePre+"out/rgbd/";
            string PosPath = FileRoot+FilePre+"out/pos/";
            //depth
            std::vector<std::filesystem::path> FileList;
            
            // png is depthData
            FileList = allFileOfDir(DepthPath,PNG);
            for(auto file:FileList){
                if(file.string().find("depth")!=std::string::npos)
                    depths[file.filename()]=DataPng(file);
            }
            FileList = allFileOfDir(PosPath,TXT);
            for(auto file:FileList){
                if(file.filename().string().find("cam")!=std::string::npos){
                    ifstream ifs(file);
                    if(!ifs){
                        throw(string("Can open file")+file.string());
                    }
                    std::vector<double> camData(9,0);
                    for(int i=0;i<9;i++){
                        ifs>>camData[i];
                    }
                    Cams[file.filename()]=Eigen::Matrix<double,3,3>(camData);
                    ifs.close();
                }
                else if(file.filename().string().find("RT")!=std::string::npos){
                    ifstream ifs(file);
                    if(!ifs){
                        throw(string("Can open file")+file.string());
                    }
                    std::vector<double> camData(12,0);
                    for(int i=0;i<12;i++){
                        ifs>>camData[i];
                    }
                    RTs[file.filename()]=Eigen::Matrix<double,3,4>(camData);
                    ifs.close();
                }
            }
        }
        const std::vector<std::vector<float>>& getDepth(const string& PicName){
            string name("rgb_");
            name = name+PicName+".png";
            return depths[name].data;
        }
        const Eigen::Matrix<double,3,3>& getCams(const string& CamName){
            string name("cam_");
            name = name+CamName+".txt";
            return Cams[name];
        }
        const Eigen::Matrix<double,3,4>& getRT(const string& RTName){
            string name("RT_");
            name = name+RTName+".txt";
            return RTs[name];
        }
        void CalAll(){

        }
        void SaveCal(string SaveRoot){

        }
        const std::vector<Eigen::Vector3f> Cal(const std::vector<std::vector<float>>& Pic, const Eigen::Matrix<float,3,3>& Cam,const Eigen::Matrix<float,3,4>& RT) {
            std::vector<Eigen::Vector3f> P;
            Eigen::Matrix<float,3,4> A=Cam*RT;
            Eigen::Vector3f x;
            for(int i=0;i<Pic.size();++i){
                for(int j=0;j<Pic[i].size();++j){
                    Eigen::Vector3f b;
                    b<<i,j,Pic[i][j];
                    x=A.lu().solve(b);
                    P.push_back(x);
                }
            }
            return P;
        }
    };
}
