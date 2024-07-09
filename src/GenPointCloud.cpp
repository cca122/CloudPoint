#include "LoadData.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <Eigen/Dense>
using namespace Eigen;
using namespace std;


const string DataRoot = "/root/nerfstudio/data/nerfstudio/RebuldData/";
const string OutRoot = "../out";
vector<string>DataPres={
    "artwork",
    "bottle"
};


int main(){
    for(int i=0;i<DataPres.size();i++){
        string DataPre = DataPres[i];
        std::vector<std::filesystem::path> DirList;
        try{
            DirList = zhywytDataLoder::allDirOfDir(DataRoot+DataPre);
        }
        catch(const char*msg){
            cout<<msg<<endl;
        }
        zhywytDataLoder::MkDir(OutRoot);
        for(auto dir:DirList){
            string prefillName = dir.filename();
            //dir.filename() is 001
            //dor os full path
            try{
                zhywytDataLoder::DataLoder loder(DataRoot,prefillName);
            }
            catch(const char*msg){
                cout<<msg<<endl;
            }
            zhywytDataLoder::DataLoder(DataRoot,prefillName);

        }
        
    }
}
