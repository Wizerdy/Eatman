#include "Utilities.h"

String getAppPath() {
    char cExeFilePath[256];
    GetModuleFileNameA(NULL, cExeFilePath, 256);
    std::string exeFilePath = cExeFilePath;
    return exeFilePath;
}

String getAssetsPath() {
    std::string exeFilePath = getAppPath();
    int exeNamePos = exeFilePath.find_last_of("\\/");
    std::string assetPath = exeFilePath.substr(0, exeNamePos + 1);
    return assetPath;
}