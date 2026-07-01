#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

const std::string HISTORY_FILE_NAME = "chat_history.txt";

// Message는 대화 한 줄의 화자와 내용을 저장하는 단순한 데이터 클래스입니다.
class Message
{
public:
    Message()
    {
    }

    Message(std::string speakerValue, std::string textValue)
    {
        speaker = speakerValue;
        text = textValue;
    }

    std::string speaker;
    std::string text;
};

// ChatBot은 사용자 입력을 보고 규칙 기반 답변을 만드는 역할을 합니다.
class ChatBot
{
public:
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
            return "메모리는 프로그램이 값을 저장하고 사용하는 공간입니다. /memory 명령어로 예제를 볼 수 있습니다.";
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
};

// HistoryManager는 대화 기록을 메모리에 보관하고 파일로 저장하거나 불러옵니다.
class HistoryManager
{
public:
    HistoryManager()
    {
        fileName = HISTORY_FILE_NAME;
        loadHistory();
    }

    void addMessage(std::string speaker, std::string text)
    {
        Message message(speaker, text);
        chatHistory.push_back(message);
    }

    void printHistory()
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

    void clearHistory()
    {
        chatHistory.clear();
        saveHistory();
    }

    void saveHistory()
    {
        std::ofstream outputFile(fileName);

        if (!outputFile)
        {
            std::cout << "대화 기록 파일을 저장할 수 없습니다." << std::endl;
            return;
        }

        // 한 줄에 화자와 내용을 구분해서 저장합니다. 예: User|안녕
        for (Message message : chatHistory)
        {
            outputFile << message.speaker << "|" << message.text << std::endl;
        }
    }

private:
    std::vector<Message> chatHistory;
    std::string fileName;

    void loadHistory()
    {
        std::ifstream inputFile(fileName);

        if (!inputFile)
        {
            // 파일이 없으면 오류가 아니라 새 대화 기록으로 시작합니다.
            return;
        }

        std::string line;

        while (std::getline(inputFile, line))
        {
            std::size_t separatorPosition = line.find("|");

            if (separatorPosition == std::string::npos)
            {
                std::cout << "읽을 수 없는 대화 기록 한 줄을 건너뜁니다." << std::endl;
            }
            else
            {
                Message message;
                message.speaker = line.substr(0, separatorPosition);
                message.text = line.substr(separatorPosition + 1);
                chatHistory.push_back(message);
            }
        }

        if (inputFile.bad())
        {
            std::cout << "대화 기록 파일을 불러오는 중 문제가 발생했습니다." << std::endl;
        }
    }
};

// CommandHandler는 /로 시작하는 명령어를 처리합니다.
class CommandHandler
{
public:
    bool isCommand(std::string input)
    {
        return input.length() > 0 && input[0] == '/';
    }

    bool handleCommand(std::string input, HistoryManager& historyManager)
    {
        if (input == "/help")
        {
            printHelp();
        }
        else if (input == "/history")
        {
            historyManager.printHistory();
        }
        else if (input == "/clear")
        {
            historyManager.clearHistory();
            std::cout << "대화 기록을 삭제했습니다." << std::endl;
        }
        else if (input == "/memory")
        {
            demonstrateMemoryConcepts();
        }
        else if (input == "/exit")
        {
            historyManager.saveHistory();
            std::cout << "AI: 프로그램을 종료합니다." << std::endl;
            return true;
        }
        else
        {
            std::cout << "알 수 없는 명령어입니다. /help를 입력해보세요." << std::endl;
        }

        return false;
    }

private:
    void printHelp()
    {
        std::cout << "사용 가능한 명령어" << std::endl;
        std::cout << "/help    : 사용 가능한 명령어를 보여줍니다." << std::endl;
        std::cout << "/history : 대화 기록을 보여줍니다." << std::endl;
        std::cout << "/clear   : 대화 기록을 삭제합니다." << std::endl;
        std::cout << "/memory  : 메모리 개념 학습 예제를 보여줍니다." << std::endl;
        std::cout << "/exit    : 프로그램을 종료합니다." << std::endl;
    }

    void demonstrateMemoryConcepts()
    {
        std::cout << "메모리 개념 학습 예제" << std::endl;
        std::cout << "이번 예제는 채팅 기능을 바꾸지 않고 C++ 메모리 개념만 관찰합니다." << std::endl;

        demonstratePassByValue();
        demonstratePassByReference();
        demonstratePointerBasic();
        demonstrateDynamicAllocation();
        demonstrateSmartPointer();
        demonstrateVectorMemory();
    }

    // 값 전달: 복사본이 전달되므로 원본 Message는 바뀌지 않습니다.
    void demonstratePassByValue()
    {
        std::cout << std::endl;
        std::cout << "[1] 값 전달 예시" << std::endl;

        Message original("User", "원본 메시지");
        Message copiedMessage = original;
        copiedMessage.text = "복사본만 수정됨";

        std::cout << "원본 text: " << original.text << std::endl;
        std::cout << "복사본 text: " << copiedMessage.text << std::endl;
        std::cout << "값 전달에서는 복사본이 바뀌어도 원본은 그대로입니다." << std::endl;
    }

    // 참조 전달: 같은 객체를 다른 이름으로 사용하므로 원본이 바뀔 수 있습니다.
    void demonstratePassByReference()
    {
        std::cout << std::endl;
        std::cout << "[2] 참조 전달 예시" << std::endl;

        Message original("User", "수정 전 메시지");
        Message& referenceMessage = original;
        referenceMessage.text = "참조를 통해 수정됨";

        std::cout << "원본 text: " << original.text << std::endl;
        std::cout << "참조 text: " << referenceMessage.text << std::endl;
        std::cout << "참조는 원본을 직접 가리키므로 원본 값이 바뀔 수 있습니다." << std::endl;
    }

    // 포인터 기본: 객체의 주소를 저장하고, -> 연산자로 값을 읽습니다.
    void demonstratePointerBasic()
    {
        std::cout << std::endl;
        std::cout << "[3] 포인터 기본 예시" << std::endl;

        Message message("AI", "포인터가 가리키는 메시지");
        Message* messagePointer = &message;

        std::cout << "Message 객체 주소: " << &message << std::endl;
        std::cout << "포인터에 저장된 주소: " << messagePointer << std::endl;
        std::cout << "포인터로 읽은 text: " << messagePointer->text << std::endl;
    }

    // 동적 할당: new로 만든 객체는 delete로 직접 해제해야 합니다.
    void demonstrateDynamicAllocation()
    {
        std::cout << std::endl;
        std::cout << "[4] 동적 할당 예시" << std::endl;

        Message* dynamicMessage = new Message("AI", "new로 만든 메시지");

        std::cout << "동적 할당 주소: " << dynamicMessage << std::endl;
        std::cout << "동적 할당 text: " << dynamicMessage->text << std::endl;
        std::cout << "new를 사용하면 delete를 빼먹을 때 메모리 누수가 생길 수 있습니다." << std::endl;

        delete dynamicMessage;
        dynamicMessage = nullptr;

        std::cout << "delete 후 포인터를 nullptr로 바꾸었습니다." << std::endl;
    }

    // 스마트 포인터: unique_ptr은 범위를 벗어날 때 자동으로 메모리를 해제합니다.
    void demonstrateSmartPointer()
    {
        std::cout << std::endl;
        std::cout << "[5] 스마트 포인터 예시" << std::endl;

        std::unique_ptr<Message> smartMessage(new Message("AI", "unique_ptr이 관리하는 메시지"));

        std::cout << "unique_ptr 주소: " << smartMessage.get() << std::endl;
        std::cout << "unique_ptr text: " << smartMessage->text << std::endl;
        std::cout << "이 함수가 끝나면 unique_ptr이 자동으로 delete를 처리합니다." << std::endl;
    }

    // vector 메모리: size는 실제 원소 수, capacity는 미리 확보한 저장 공간입니다.
    void demonstrateVectorMemory()
    {
        std::cout << std::endl;
        std::cout << "[6] vector 크기와 capacity 관찰 예시" << std::endl;

        std::vector<Message> messages;

        std::cout << "처음 size: " << messages.size() << ", capacity: " << messages.capacity() << std::endl;

        messages.push_back(Message("User", "첫 번째 메시지"));
        std::cout << "1개 추가 후 size: " << messages.size() << ", capacity: " << messages.capacity() << std::endl;

        messages.push_back(Message("AI", "두 번째 메시지"));
        std::cout << "2개 추가 후 size: " << messages.size() << ", capacity: " << messages.capacity() << std::endl;

        messages.push_back(Message("User", "세 번째 메시지"));
        std::cout << "3개 추가 후 size: " << messages.size() << ", capacity: " << messages.capacity() << std::endl;
        std::cout << "capacity는 vector가 내부 저장 공간을 늘릴 때 바뀔 수 있습니다." << std::endl;
    }
};

// ChatApp은 전체 프로그램 실행 흐름을 담당합니다.
class ChatApp
{
public:
    ChatApp()
    {
    }

    void run()
    {
        printWelcomeMessage();

        while (true)
        {
            std::string input = getUserInput();

            if (commandHandler.isCommand(input))
            {
                bool shouldExit = commandHandler.handleCommand(input, historyManager);

                if (shouldExit)
                {
                    break;
                }
            }
            else
            {
                std::string response = chatBot.getResponse(input);

                historyManager.addMessage("User", input);
                historyManager.addMessage("AI", response);
                printBotResponse(response);
            }
        }
    }

private:
    ChatBot chatBot;
    HistoryManager historyManager;
    CommandHandler commandHandler;

    void printWelcomeMessage()
    {
        std::cout << "LocalAIChat 8단계 기본 채팅 프로그램" << std::endl;
        std::cout << "문장을 입력하면 간단한 규칙으로 응답합니다." << std::endl;
        std::cout << "메모리 학습 예제를 보려면 /memory를 입력하세요." << std::endl;
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

    void printBotResponse(std::string response)
    {
        std::cout << "AI: " << response << std::endl;
    }
};

int main()
{
    ChatApp app;
    app.run();

    return 0;
}