#include <iostream>
#include <string>
#include <vector>

struct Message
{
    std::string speaker;
    std::string text;
};

void printWelcomeMessage();
std::string getUserInput();
bool isExitCommand(std::string input);
bool isHistoryRequest(std::string input);
std::string getResponse(std::string input);
void printBotResponse(std::string response);
void addMessage(std::vector<Message>& chatHistory, std::string speaker, std::string text);
void printChatHistory(std::vector<Message> chatHistory);

int main()
{
    std::vector<Message> chatHistory;

    printWelcomeMessage();

    while (true)
    {
        std::string input = getUserInput();

        if (isExitCommand(input))
        {
            printBotResponse("프로그램을 종료합니다.");
            break;
        }

        if (isHistoryRequest(input))
        {
            printChatHistory(chatHistory);
        }
        else
        {
            std::string response = getResponse(input);

            addMessage(chatHistory, "User", input);
            addMessage(chatHistory, "AI", response);
            printBotResponse(response);
        }
    }

    return 0;
}

void printWelcomeMessage()
{
    std::cout << "LocalAIChat 3단계 기본 채팅 프로그램" << std::endl;
    std::cout << "문장을 입력하면 간단한 규칙으로 응답합니다." << std::endl;
    std::cout << "대화 기록을 보려면 기록 또는 history를 입력하세요." << std::endl;
    std::cout << "종료하려면 exit를 입력하세요." << std::endl;
    std::cout << std::endl;
}

std::string getUserInput()
{
    std::string input;

    std::cout << "User: ";
    std::getline(std::cin, input);

    return input;
}

bool isExitCommand(std::string input)
{
    return input == "exit";
}

bool isHistoryRequest(std::string input)
{
    return input == "기록" || input == "history";
}

std::string getResponse(std::string input)
{
    // find는 문자열 안에 특정 단어가 있는지 찾을 때 사용합니다.
    if (input.find("안녕") != std::string::npos)
    {
        return "안녕하세요! C++ 채팅 프로그램에 오신 것을 환영합니다.";
    }
    else if (input.find("C++") != std::string::npos)
    {
        return "C++ 기초를 배우고 있군요. 반복문, 조건문, 함수를 차근차근 익혀봅시다.";
    }
    else if (input.find("도움") != std::string::npos)
    {
        return "사용할 수 있는 입력 예시는 안녕, C++, 도움, 기록, history, exit입니다.";
    }

    return "아직은 간단한 규칙으로만 대답할 수 있어요. '도움'을 입력해보세요.";
}

void printBotResponse(std::string response)
{
    std::cout << "AI: " << response << std::endl;
}

void addMessage(std::vector<Message>& chatHistory, std::string speaker, std::string text)
{
    Message message;
    message.speaker = speaker;
    message.text = text;

    chatHistory.push_back(message);
}

void printChatHistory(std::vector<Message> chatHistory)
{
    std::cout << "대화 기록" << std::endl;

    if (chatHistory.empty())
    {
        std::cout << "저장된 대화 기록이 없습니다." << std::endl;
        return;
    }

    for (Message message : chatHistory)
    {
        std::cout << message.speaker << ": " << message.text << std::endl;
    }
}