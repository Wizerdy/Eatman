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

float ParticleSize(int size) {
	switch (size) {
	case 0:
	case 1:
		return 0.7f;
	case 2:
		return 1.5f;
	case 3:
		return 2.7f;
	default:
		return 2.7f;
	}
}

int Superior(int a, int b) {
	if (a > b) {
		return a;
	}
	return b;
}


int Inferior(int a, int b) {
	if (a > b) {
		return b;
	}
	return a;
}

int Sign(float a, float b) {
	if (a > b) {
		return 1;
	}
	else if (a == b) {
		return 0;
	}
	return -1;
}