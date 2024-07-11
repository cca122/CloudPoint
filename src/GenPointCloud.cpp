#include <iostream>
#include <fstream>
#include <vector>
#include <Eigen/Dense>
#include "LoadData.hpp"
using namespace Eigen;
using namespace std;

static size_t LenOfLog = 1;
static size_t LenOfError = 1;
bool OUT_LOG=true;
bool OUT_ERROR=true;
const string DataRoot = "/root/nerfstudio/data/nerfstudio/RebuldData/";
const string OutRoot = "../out/";
vector<string>DataPres={
    "artwork/",
    "bottle/"
};


void MyLog(string msg){
    if(OUT_LOG)
        cout<<"[Log: "<< LenOfLog++ <<"]"<< msg<<endl;
}
void MyError(string msg){
    if(OUT_ERROR)
        cerr<<"[Error :"<< LenOfError++ <<"]"<<msg<<endl;
}
int main(int argc,char **argv){
    if(argc>=3)OUT_LOG=false;
    if(argc>=2)OUT_ERROR=false;
    for(int i=0;i<DataPres.size();i++){
        auto t0 = omp_get_wtime();
        MyLog(string("number of available processors: ")+to_string(omp_get_num_procs()));
        MyLog(string("number of threads: ")+to_string(omp_get_max_threads()));
        string DataPre = DataPres[i];
        std::vector<std::filesystem::path> DirList;
        MyLog(string("Begin search ")+DataPre);
        try{
            DirList = zhywytDataLoder::allDirOfDir(DataRoot+DataPre);
            MyLog(string("File numbers:")+to_string(DirList.size()));
            MyLog("Search file done!");
        }
        catch(const char*msg){
            MyError(string(msg));
            MyError(string("Can not search file with prefix: ")+DataPre);
        }
        zhywytDataLoder::MkDir(OutRoot);
        for(auto dir:DirList){
            string prefillName = DataPre + dir.filename().string()+string("/");
            MyLog(string("Begin handle object of ")+prefillName);
            //dir.filename() is 001
            //dor os full path
            try{
                // OUT_LOG=false;
                zhywytDataLoder::DataLoder loder(DataRoot,prefillName);
                OUT_LOG=true;
                MyLog(string("Load object done!"));
                MyLog(string("Begin Cal PointCloud"));
                loder.CalAll();
                MyLog(string("Cal CloudPoint done!"));
                MyLog(string("Begin Save CloudPoint"));
                loder.SaveCal(OutRoot+DataPre);
                MyLog(string("Save CloudPoint done!"));
                MyLog(string("Handle object done!"));
            }catch(const char*msg){
                MyError(msg);
                MyError(string("Can not handle the object: ")+prefillName);
            }catch(const std::out_of_range& e) {
                // 捕获 std::string 越界访问异常
                MyError(e.what());
                MyError(string("Caught out_of_range exception: ")+prefillName);
            }catch(const std::bad_alloc& e) {
                // 捕获内存分配失败异常
                MyError(e.what());
                MyError(string("Caught bad_alloc exception: ")+prefillName);
            }catch(const std::exception& e) {
                MyError(e.what());
                MyError(string("Can not handle the object: ")+prefillName);
            }
        }
        auto t1 = omp_get_wtime();
        MyLog(DataPre+string(" use time: ")+to_string(t1-t0)+" seconds");
    }
}
