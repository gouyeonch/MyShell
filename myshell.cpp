#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

void executeCommand(const std::vector<std::string>& args, bool runInBackground = false, int pipeTo = -1) {
    pid_t pid = fork();

    if (pid == -1) {
        std::cerr << "프로세스를 생성할 수 없습니다." << std::endl;
        return;
    } else if (pid == 0) {
        // 자식 프로세스에서 명령어 실행

        // 파이프 설정
        if (pipeTo != -1) {
            dup2(pipeTo, STDOUT_FILENO);
            close(pipeTo);
        }

        // 백그라운드 실행 시 표준 입력 및 출력 리디렉션
        if (runInBackground) {
            int devNull = open("/dev/null", O_RDWR);
            dup2(devNull, STDIN_FILENO);
            dup2(devNull, STDOUT_FILENO);
            dup2(devNull, STDERR_FILENO);
            close(devNull);
        }

        // 명령어 실행
        std::vector<char*> cArgs;
        for (const auto& arg : args) {
            cArgs.push_back(const_cast<char*>(arg.c_str()));
        }
        cArgs.push_back(nullptr);

        execvp(cArgs[0], cArgs.data());
        std::cerr << "명령어를 실행할 수 없습니다." << std::endl;
        exit(EXIT_FAILURE);
    } else {
        // 부모 프로세스에서 자식 프로세스의 종료를 기다리거나 백그라운드로 실행

        if (!runInBackground) {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

void executeCommand(const std::string& command, bool runInBackground = false, int pipeTo = -1) {
    std::vector<std::string> args;
    std::istringstream iss(command);
    std::string arg;
    while (iss >> arg) {
        args.push_back(arg);
    }
    executeCommand(args, runInBackground, pipeTo);
}

void listFiles(const std::string& directory) {
    DIR* dir = opendir(directory.c_str());
    if (dir == nullptr) {
        std::cerr << "디렉토리를 열 수 없습니다." << std::endl;
        return;
    }

    dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::cout << entry->d_name << std::endl;
    }

    closedir(dir);
}

void changeDirectory(const std::string& directory) {
    if (chdir(directory.c_str()) != 0) {
        std::cerr << "디렉토리를 변경할 수 없습니다." << std::
        std::cerr << "디렉토리를 변경할 수 없습니다." << std::endl;
    }
}

std::string getCurrentDirectory() {
    char buffer[PATH_MAX];
    if (getcwd(buffer, PATH_MAX) == nullptr) {
        std::cerr << "현재 디렉토리를 가져올 수 없습니다." << std::endl;
        return "";
    }

    return std::string(buffer);
}

void createDirectory(const std::string& directory) {
    if (mkdir(directory.c_str(), 0777) != 0) {
        std::cerr << "디렉토리를 생성할 수 없습니다." << std::endl;
    }
}

void removeFile(const std::string& filename) {
    if (remove(filename.c_str()) != 0) {
        std::cerr << "파일을 삭제할 수 없습니다." << std::endl;
    }
}

void grep(const std::string& pattern, const std::string& filename) {
    std::vector<std::string> args = {"grep", pattern, filename};
    executeCommand(args);
}

void ps() {
    std::vector<std::string> args = {"ps"};
    executeCommand(args);
}

void killProcess(int pid) {
    std::vector<std::string> args = {"kill", std::to_string(pid)};
    executeCommand(args);
}

void changeFilePermissions(const std::string& filename, const std::string& permissions) {
    std::vector<std::string> args = {"chmod", permissions, filename};
    executeCommand(args);
}

void createTarArchive(const std::string& filename, const std::vector<std::string>& files) {
    std::vector<std::string> args = {"tar", "-cvf", filename};
    args.insert(args.end(), files.begin(), files.end());
    executeCommand(args);
}

void extractTarArchive(const std::string& filename) {
    std::vector<std::string> args = {"tar", "-xvf", filename};
    executeCommand(args);
}

void sed(const std::string& pattern, const std::string& replacement, const std::string& filename) {
    std::vector<std::string> args = {"sed", "-i", "s/" + pattern + "/" + replacement + "/g", filename};
    executeCommand(args);
}

void awk(const std::string& pattern, const std::string& filename) {
    std::vector<std::string> args = {"awk", pattern, filename};
    executeCommand(args);
}

void curl(const std::string& url) {
    std::vector<std::string> args = {"curl", url};
    executeCommand(args);
}

void find(const std::string& directory, const std::string& filename) {
    std::vector<std::string> args = {"find", directory, "-name", filename};
    executeCommand(args);
}

void sortFile(const std::string& filename) {
    std::vector<std::string> args = {"sort", filename};
    executeCommand(args);
}

void renameFile(const std::string& oldname, const std::string& newname) {
    std::vector<std::string> args = {"mv", oldname, newname};
    executeCommand(args);
}

void moveFile(const std::string& source, const std::string& destination) {
    std::vector<std::string> args = {"mv", source, destination};
    executeCommand(args);
}

void copyFile(const std::string& source, const std::string& destination) {
    std::vector<std::string> args = {"cp", source, destination};
   
    executeCommand(args);
}

int main() {
    std::string command;

    while (true) {
        std::cout << "명령어 입력: ";
        std::getline(std::cin, command);

        if (command == "exit") {
            break;
        } else if (command.empty()) {
            continue;
        } else if (command.substr(0, 3) == "ls ") {
            std::string directory = command.substr(3);
            listFiles(directory);
        } else if (command.substr(0, 3) == "cd ") {
            std::string directory = command.substr(3);
            changeDirectory(directory);
        } else if (command == "pwd") {
            std::string currentDirectory = getCurrentDirectory();
            std::cout << currentDirectory << std::endl;
        } else if (command.substr(0, 6) == "mkdir ") {
            std::string directory = command.substr(6);
            createDirectory(directory);
        } else if (command.substr(0, 3) == "rm ") {
            std::string filename = command.substr(3);
            removeFile(filename);
        } else if (command.substr(0, 5) == "grep ") {
            std::string patternAndFile = command.substr(5);
            size_t spacePos = patternAndFile.find(' ');
            std::string pattern = patternAndFile.substr(0, spacePos);
            std::string filename = patternAndFile.substr(spacePos + 1);
            grep(pattern, filename);
        } else if (command == "ps") {
            ps();
        } else if (command.substr(0, 5) == "kill ") {
            int pid = std::stoi(command.substr(5));
            killProcess(pid);
        } else if (command.substr(0, 6) == "chmod ") {
            std::string args = command.substr(6);
            size_t spacePos = args.find(' ');
            std::string permissions = args.substr(0, spacePos);
            std::string filename = args.substr(spacePos + 1);
            changeFilePermissions(filename, permissions);
        } else if (command.substr(0, 4) == "tar ") {
            std::string args = command.substr(4);
            size_t spacePos = args.find(' ');
            std::string option = args.substr(0, spacePos);
            std::string filename = args.substr(spacePos + 1);
            if (option == "-c") {
                std::vector<std::string> files;
                while (std::getline(std::cin, command) && command != "") {
                    files.push_back(command);
                }
                createTarArchive(filename, files);
            } else if (option == "-x") {
                extractTarArchive(filename);
            }
        } else if (command.substr(0, 4) == "sed ") {
            std::string args = command.substr(4);
            size_t firstSpacePos = args.find(' ');
            size_t secondSpacePos = args.find(' ', firstSpacePos + 1);
            std::string pattern = args.substr(0, firstSpacePos);
            std::string replacement = args.substr(firstSpacePos + 1, secondSpacePos - firstSpacePos - 1);
            std::string filename = args.substr(secondSpacePos + 1);
            sed(pattern, replacement, filename);
        } else if (command.substr(0, 4) == "awk ") {
            std::string args = command.substr(4);
            size_t spacePos = args.find(' ');
            std::string pattern = args.substr(0, spacePos);
            std::string filename = args.substr
            awk(pattern, filename);
        } else if (command.substr(0, 5) == "curl ") {
            std::string url = command.substr(5);
            curl(url);
        } else if (command.substr(0, 5) == "find ") {
            std::string args = command.substr(5);
            size_t spacePos = args.find(' ');
            std::string directory = args.substr(0, spacePos);
            std::string filename = args.substr(spacePos + 1);
            find(directory, filename);
        } else if (command.substr(0, 5) == "sort ") {
            std::string filename = command.substr(5);
            sortFile(filename);
        } else if (command.substr(0, 7) == "rename ") {
            std::string args = command.substr(7);
            size_t spacePos = args.find(' ');
            std::string oldname = args.substr(0, spacePos);
            std::string newname = args.substr(spacePos + 1);
            renameFile(oldname, newname);
        } else if (command.substr(0, 5) == "move ") {
            std::string args = command.substr(5);
            size_t spacePos = args.find(' ');
            std::string source = args.substr(0, spacePos);
            std::string destination = args.substr(spacePos + 1);
            moveFile(source, destination);
        } else if (command.substr(0, 3) == "cp ") {
            std::string args = command.substr(3);
            size_t spacePos = args.find(' ');
            std::string source = args.substr(0, spacePos);
            std::string destination = args.substr(spacePos + 1);
            copyFile(source, destination);
        } else {
            executeCommand(command);
        }
    }

    return 0;
}
