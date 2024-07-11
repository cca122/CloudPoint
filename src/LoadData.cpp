#include "LoadData.hpp"
extern void MyLog(string msg);
extern void MyError(string msg);
extern bool OUT_LOG,OUT_ERROR;
enum TYPE{
        CSV,
        TXT,
        PNG
    };
std::map<int,string>tailMap={
    {CSV,".scv"},
    {TXT,".txt"},
    {PNG,".png"}
};
namespace zhywytDataLoder{
    void DataLoder::SaveCal(string SaveRoot){
        std::filesystem::path PcdFilePath(SaveRoot);
        MyLog(string("In fun SaveCal() PicList size: ")+std::to_string(PicListId.size()));
        for(auto PicID:PicListId){
            std::filesystem::path tmp = PcdFilePath.string()+"/"+std::to_string(PicID)+".obj";
            MyLog(string("Saving file ")+tmp.string());
            auto& data = PcdData[std::to_string(PicID)];
            //data like vector<vector<int>> data(n,vector<int>(6))
            //TODO : save the data to file tmp.string()
            std::ofstream ofs(tmp.string());
            if(!ofs){
                throw(string("Can't open file")+tmp.string());
            }
            for(size_t i=0;i<data.size();i++){
                ofs<<"v";
                for(int j=0;j<6;j++){
                    ofs<<" "<<data[i][j];
                }
                ofs<<"\n";
            }
            
        }
    }
    void DataLoder::CalAll(){
        MyLog(string("In fun CalAll() The Pic num: ")+std::to_string(PicListId.size()));
        for(auto id : PicListId){
            string ID = std::to_string(id);
            int r = id/100;
            int g = id/10%10;
            int b = id%10;
            MyLog(string("Start Cal ")+ID);
            std::vector<Eigen::Vector3f> p;
            try{
                p = Cal(ID);
            }catch(std::runtime_error e){
                MyError(e.what());
            }
            MyLog(string("End Cal ")+ID);
            for(const auto& pp:p){
                std::vector<int>line;
                line[0]=pp[0];
                line[1]=pp[1];
                line[2]=pp[2];
                line[3]=r;
                line[4]=g;
                line[5]=b;
                PcdData[ID].push_back(line);
            }
        }
    }
    DataLoder::DataLoder(const string& fileRoot, const string& filePre):FilePre(filePre),FileRoot(fileRoot){
        string DepthPath = FileRoot+FilePre+"out/rgbd/";
        string PosPath = FileRoot+FilePre+"out/pose/";
        //depth
        std::vector<std::filesystem::path> FileList;
        MyLog(string("In fun DataLoder() Begin search ")+DepthPath);
        // png is depthData
        try{
            FileList = allFileOfDir(DepthPath,PNG);
        }catch(const char*msg){
            MyError(msg);
        }catch(const std::exception& e) {
            MyError(e.what());
        }
        MyLog(string("In fun DataLoder() Pic Number: ")+std::to_string(FileList.size()));
        OUT_LOG=false;
        PicListId.reserve(FileList.size());
        // #pragma omp parallel for shared(PicListId)
        for(auto file:FileList){
            string ID = file.filename().string();
            MyLog(string("In fun DataLoder() Begin handle Pic: ")+file.string());
            ID = ID.substr(ID.find("_")+1,ID.find(".")-ID.find("_")-1);
            PicListId.push_back(std::stoi(ID));
            MyLog(string("In fun DataLoder() Solvnig the file name:")+ID);
            if(file.string().find("depth")!=std::string::npos){
                try{
                    depths[ID]=std::move(DataPng(file));
                }catch(const char*msg){
                    MyError(msg);
                }catch(...){
                    // MyError("Unexcept Error in DataLoder()");
                }
            }
            MyLog(string("In fun DataLoder() Solved the file name: ")+ID);
        }
        OUT_LOG=true;
        MyLog(string("-----Solved all Pic-----"));
        MyLog(string("In fun DataLoder() Begin search ")+PosPath);
        try{
            FileList = allFileOfDir(PosPath,TXT);
        }catch(const char*msg){
            MyError(msg);
        }catch(const std::exception& e) {
            MyError(e.what());
        }
        MyLog(string("In fun DataLoder() Pose Number: ")+std::to_string(FileList.size()));
        OUT_LOG=false;
        // #pragma omp parallel for shared(Cams)
        for(auto file:FileList){
            string ID = file.filename().string();
            MyLog(string("In fun DataLoder() Begin handle Cam: ")+file.string());
            ID = ID.substr(ID.find("_")+1,ID.find(".")-ID.find("_")-1);
            MyLog(string("In fun DataLoder() Solvnig the file name:")+ID);
            if(file.filename().string().find("cam")!=std::string::npos){
                ifstream ifs(file);
                if(!ifs){
                    throw(string("In fun DataLoder() Can open file")+file.string());
                }
                Eigen::Matrix<float,3,3> camData;
                for(int i=0;i<9;i++){
                    ifs>>camData(i/3,i%3);
                }
                Cams[ID]=camData;
                ifs.close();
            }
            else if(file.filename().string().find("RT")!=std::string::npos){
                ifstream ifs(file);
                if(!ifs){
                    throw(string("In fun DataLoder() Can open file")+file.string());
                }
                Eigen::Matrix<float,3,4> camData;
                for(int i=0;i<12;i++){
                    ifs>>camData(i/4,i%4);
                }
                RTs[ID]=Eigen::Matrix<float,3,4>(camData);
                ifs.close();
            }
        }
        OUT_LOG=true;
        MyLog(string("-----Solved all Pose-----"));
    }
    DataPng::DataPng(std::filesystem::path FilePath=""):path(FilePath){
        cv::Mat depthImage = cv::imread(FilePath,cv::IMREAD_UNCHANGED);     //cv::IMREAD_UNCHANGED确保不改变图像的原始格式
        if(depthImage.empty()){
            throw(string("Can't open file")+FilePath.string());
        }
        if (depthImage.type() != CV_16U && depthImage.type() != CV_32F) {
            throw("Depth image is not in a supported format.");
        }
        height = depthImage.rows;
        width = depthImage.cols;
        MyLog(string("Height : ")+std::to_string(height)+" Width : "+std::to_string(width));
        for (int i = 0; i < height; i++){
            std::vector<float>tmp;
            for (int j = 0; j < width; j++){
                float dep = static_cast<int>(depthImage.at<uchar>(i,j));
                tmp.push_back(dep);
            }
            data.push_back(tmp);
        }
        Cast();
        MyLog(string("Finished load Pic ")+FilePath.string());
    }
inline std::vector<Eigen::Vector3f> DataLoder::Cal(string ID) {
    Eigen::Matrix<float,3,4> RT;
    std::vector<std::vector<float>> Pic;
    Eigen::Matrix<float,3,3>Cam;
    RT = RTs[ID];
    Pic = depths[ID].data;
    Cam = Cams[ID];
    // auto RT = RTs[ID];
    
    std::vector<Eigen::Vector3f> P;

    for (int i = 0; i < Pic.size(); ++i) {
        for (int j = 0; j < Pic[i].size(); ++j) {
            Eigen::Vector4f b;
            Eigen::Matrix<float, 4, 4> A = Eigen::Matrix<float,4,4>::Identity();
            Eigen::Vector4f x;
            A.block<3,4>(1,1) = Cam*RT;
            b << i,j,Pic[i][j],1;

            x = A.fullPivLu().solve(b);
            Eigen::Vector3f tmp;
            tmp << x(1, 1), x(2, 1), x(3, 1);
            P.push_back(tmp);
        }
    }
    return P;
}
    void DataPng::Cast(){//Cast the 
        for(auto& ii:data){
            for(auto& i:ii){
                i = i/ 65535.0 * 3000.0; 
            }
        }
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
bool fileIsExist(string fileName){
    // 0 /* Test for existence.  */
    return access(fileName.c_str(),F_OK)==0;
}
    bool MkDir(const std::string& strPath) {
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
    bool RmDir(const std::string & path) {
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
}
