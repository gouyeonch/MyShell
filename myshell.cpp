#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

void executeCommand(const std::string& command, bool runInBackground = false, int pipeTo = -1) {
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
        std::vector<char*> args;
        char* token = std::strtok(const_cast<char*>(command.c_str()), " ");
        while (token != nullptr) {
            args.push_back(token);
            token = std::strtok(nullptr, " ");
        }
        args.push_back(nullptr);

        execvp(args[0], args.data());
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

int main() {
    std::string command;

    std::cout << "쉘 프로그램을 실행합니다." << std::endl;

    while (true) {
        std::cout << "명령어를 입력하세요 ('exit'를 입력하면 종료): ";
        std::getline(std::cin, command);

        if (command == "exit") {
            break;
        } else if (command.empty()) {
            continue;
        }

        // 파이프 기능 구현
        size_t pipePos = command.find("|");
        if (pipePos != std::string::npos) {
            std::string firstCommand = command.substr(0, pipePos);
            std::string secondCommand = command.substr(pipePos + 1);

            int pipefd[2];
            if (pipe(pipefd) == -1) {
                std::cerr << "파이프를 생성할 수 없습니다." << std::endl;
                continue;
            }

            // 첫 번째 명령어 실행
            executeCommand(firstCommand, false, pipefd[1]);
            close(pipefd[1]);

            // 두 번째 명령어 실행
            executeCommand(secondCommand, false, pipefd[0]);
            close(pipefd[0]);
        } else {
            // 백그라운드 실행 기능 구현
            bool runInBackground = false;
            if (command.back() == '&') {
                runInBackground = true;
                command.pop_back();
            }

            executeCommand(command, runInBackground);
        }

        std::cout << std::endl;
    }

    std::cout << "쉘 프로그램 종료" << std::endl;
}