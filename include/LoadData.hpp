#ifndef LOAD_DATA
#define LOAD_DATA
#include <Eigen/Dense>
#include <Eigen/LU>
#include <opencv2/opencv.hpp>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <omp.h>
using std::ifstream,std::string;
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
// 版权声明：本文为博主原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接和本声明。
// 原文链接：https://blog.csdn.net/venom_snake/article/details/88066475
namespace zhywytDataLoder{
    enum TYPE{
        CSV,
        TXT,
        PNG
    };
    bool MkDir(const std::string& strPath);
    bool RmDir(const std::string & path);
    bool fileIsExist(string fileName);
    std::vector<std::filesystem::path> allFileOfDir(string dir,int type);
    std::vector<std::filesystem::path> allDirOfDir(string dir);
    // functionDef
    class DataLoder;
    bool fileIsExist(string fileName);
    // type is file tail like .txt .csv
    std::vector<std::filesystem::path> allFileOfDir(string dir,int type);
    std::vector<std::filesystem::path> allDirOfDir(string dir);
    /*
    每个DataLoder读入一个物体，并生成对应的数据结构，数据结构设计
    */
   class DataPng{
            int height,width;
            string name;
        public:
            std::vector<std::vector<float>> data;
            std::filesystem::path path;
            void Cast();
            DataPng(std::filesystem::path FilePath);
        };
    class DataLoder{
    private:
        
        // use artworkxxx or botxxx as title
        // filename, data
        std::map<string,DataPng>depths;
        std::map<string,Eigen::Matrix<float,3,3> >Cams;
        std::map<string,Eigen::Matrix<float,3,4> >RTs;
        std::vector<int>PicListId;
        std::map<string,std::vector<std::vector<int>>>PcdData;
        string FileRoot;
        string FilePre;
    public:
        std::map<int,string>tailMap={
            {CSV,".scv"},
            {TXT,".txt"},
            {PNG,".png"}
        };
        void SaveCal(string SaveRoot);
        void CalAll();
        // is one artwork | bottle with many picture
        DataLoder(const string& fileRoot, const string& filePre);
        inline std::vector<Eigen::Vector3f> Cal(string ID);
    };
}
#endif