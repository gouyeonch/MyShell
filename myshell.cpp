#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

int main() {
    std::string command;

    while (true) {
        std::cout << "명령어를 입력하세요 (종료하려면 'exit' 입력): ";
        std::getline(std::cin, command);

        if (command == "exit") {
            break;
        }

        // 문자열을 C 스타일의 문자열로 변환
        char cmd[command.length() + 1];
        strcpy(cmd, command.c_str());

        // 명령어 실행
        int result = system(cmd);

        // 실행 결과 출력
        std::cout << "실행 결과: " << result << std::endl;
        std::cout << std::endl;
    }

    return 0;
}
