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
bool isCommand(std::string input);
bool handleCommand(std::string input, std::vector<Message>& chatHistory);
std::string getResponse(std::string input);
void printBotResponse(std::string response);
void addMessage(std::vector<Message>& chatHistory, std::string speaker, std::string text);
void printChatHistory(std::vector<Message> chatHistory);
void printHelp();

int main()
{
    std::vector<Message> chatHistory;

    printWelcomeMessage();

    while (true)
    {
        std::string input = getUserInput();

        if (isCommand(input))
        {
            bool shouldExit = handleCommand(input, chatHistory);

            if (shouldExit)
            {
                break;
            }
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
    std::cout << "LocalAIChat 5단계 기본 채팅 프로그램" << std::endl;
    std::cout << "문장을 입력하면 간단한 규칙으로 응답합니다." << std::endl;
    std::cout << "사용 가능한 명령어를 보려면 /help를 입력하세요." << std::endl;
    std::cout << "종료하려면 /exit를 입력하세요." << std::endl;
    std::cout << std::endl;
}

std::string getUserInput()
{
    std::string input;

    std::cout << "User: ";
    std::getline(std::cin, input);

    return input;
}

bool isCommand(std::string input)
{
    return input.length() > 0 && input[0] == '/';
}

bool handleCommand(std::string input, std::vector<Message>& chatHistory)
{
    if (input == "/help")
    {
        printHelp();
    }
    else if (input == "/history")
    {
        printChatHistory(chatHistory);
    }
    else if (input == "/clear")
    {
        chatHistory.clear();
        std::cout << "대화 기록을 삭제했습니다." << std::endl;
    }
    else if (input == "/exit")
    {
        printBotResponse("프로그램을 종료합니다.");
        return true;
    }
    else
    {
        std::cout << "알 수 없는 명령어입니다. /help를 입력해보세요." << std::endl;
    }

    return false;
}

std::string getResponse(std::string input)
{
    // 여러 키워드가 들어 있어도 위에서부터 먼저 발견된 규칙 하나만 선택합니다.
    if (input.find("안녕") != std::string::npos)
    {
        return "안녕하세요! C++ 학습용 LocalAIChat입니다.";
    }
    else if (input.find("C++") != std::string::npos)
    {
        return "C++은 변수, 조건문, 반복문, 함수부터 차근차근 익히면 좋습니다.";
    }
    else if (input.find("문법") != std::string::npos)
    {
        return "문법은 프로그램을 쓰는 기본 규칙입니다. 작은 예제를 직접 실행해보며 익혀보세요.";
    }
    else if (input.find("자료구조") != std::string::npos)
    {
        return "자료구조는 데이터를 정리하는 방법입니다. 지금 단계에서는 vector로 대화 기록을 저장하고 있습니다.";
    }
    else if (input.find("메모리") != std::string::npos)
    {
        return "메모리는 프로그램이 값을 저장하고 사용하는 공간입니다. 포인터와 동적 할당은 나중 단계에서 다룹니다.";
    }
    else if (input.find("알고리즘") != std::string::npos)
    {
        return "알고리즘은 문제를 해결하는 절차입니다. 지금은 키워드를 찾고 답변을 고르는 간단한 규칙을 사용합니다.";
    }
    else if (input.find("프로그램 구조") != std::string::npos)
    {
        return "프로그램 구조를 나누면 main 함수는 흐름을 보고, 세부 작업은 함수가 맡게 할 수 있습니다.";
    }
    else if (input.find("도움") != std::string::npos)
    {
        return "사용할 수 있는 키워드는 안녕, C++, 문법, 자료구조, 메모리, 알고리즘, 프로그램 구조, 도움입니다. 명령어는 /help를 입력해 확인하세요.";
    }

    return "아직 아는 키워드가 아니에요. '도움'을 입력하면 사용할 수 있는 키워드를 볼 수 있습니다.";
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

void printHelp()
{
    std::cout << "사용 가능한 명령어" << std::endl;
    std::cout << "/help    : 사용 가능한 명령어를 보여줍니다." << std::endl;
    std::cout << "/history : 대화 기록을 보여줍니다." << std::endl;
    std::cout << "/clear   : 대화 기록을 삭제합니다." << std::endl;
    std::cout << "/exit    : 프로그램을 종료합니다." << std::endl;
}