#include <iostream>
#include <vector>
#include <functional>
#include <cstdlib>
#include <variant>
#include <ctime>
#include <fstream>
#include <Windows.h>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

using namespace std;
using namespace rapidjson;


Document read_config(const char* path, char* readBuffer) {
    // �򿪲���ȡ�����ļ�
    FILE* fp;
    errno_t err = fopen_s(&fp, path, "rb"); 

    if (err != 0 || !fp) {
        cerr << "Failed to open config file" << '\n';
        return NULL;
    }

    // ʹ�ö�̬�ڴ����������readBuffer
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(char) * 65536);

    Document d;
    d.ParseStream(is);
    fclose(fp);

    // ȷ���ĵ������ɹ���Ϊ��������
    if (!d.IsObject()) {
        std::cerr << "JSON document is not an object." << '\n';
        return NULL;
    }

    return d;
}

HINSTANCE load_library(Document d){
    // ���ض�̬��
    // ȷ�������д��� "TestedLibraryPath"
    if (!d.HasMember("TestConfiguration") || !d["TestConfiguration"].IsObject() ||
        !d["TestConfiguration"].HasMember("TestedLibraryPath")) {
        std::cerr << "TestedLibraryPath not found in configuration." << '\n';
        return NULL;
    }

    const rapidjson::Value& config = d["TestConfiguration"];
    std::string libraryPath = config["TestedLibraryPath"].GetString();
    HINSTANCE hinstLib = LoadLibraryA(libraryPath.c_str());
    //HINSTANCE hinstLib = LoadLibrary(TEXT("G:\\final\\vs2010\\project\\computeEIRP\\x64\\Release\\computeEIRP.dll"));
    if (hinstLib == NULL) {
        std::cerr << "Cannot open library: " << libraryPath << '\n';
        return NULL;
    }
    return hinstLib;
}