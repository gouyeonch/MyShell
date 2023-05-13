#include <iostream>
#include <dirent.h>
#include <sys/stat.h>

using namespace std;

void listFiles(const string& directory) {
    DIR* dir;
    struct dirent* entry;
    struct stat fileStat;

    dir = opendir(directory.c_str());
    if (dir == nullptr) {
        std::cout << "디렉토리를 열 수 없습니다." << std::endl;
        return;
    }

    while ((entry = readdir(dir)) != nullptr) {
        string filename = entry->d_name;
        string filepath = directory + "/" + filename;

        if (stat(filepath.c_str(), &fileStat) == -1) {
            continue;
        }

        // 디렉토리인 경우 디렉토리 표시 추가
        if (S_ISDIR(fileStat.st_mode)) {
            filename += "/";
        }

        std::cout << filename << std::endl;
    }

    closedir(dir);
}