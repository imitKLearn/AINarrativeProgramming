# LocalAIChat

LocalAIChat은 C++ 기반 로컬 AI 채팅 프로그램을 단계적으로 만들기 위한 학습용 프로젝트입니다. 최종 목표는 C++ 문법, 프로그램 구조, 자료구조, 메모리, 알고리즘의 기초를 익히면서 작은 콘솔 프로그램을 점진적으로 확장하는 것입니다.

## 현재 단계

2단계: 함수 중심으로 프로그램 구조 정리

현재 단계에서는 1단계의 콘솔 기반 기본 채팅 동작을 유지하면서 코드를 함수 중심으로 더 읽기 쉽게 정리합니다. 실제 AI 모델을 연결하지 않고, 사용자가 입력한 문장에 대해 간단한 규칙 기반 응답을 출력합니다.

## 현재 폴더 위치

```text
D:\KM\K82\NA\NA
```

## 프로젝트 파일 구조

```text
D:\KM\K82\NA\NA\
├─ LocalAIChat.sln
├─ README.md
├─ AGENTS.md
├─ .gitignore
└─ LocalAIChat\
   ├─ LocalAIChat.vcxproj
   ├─ LocalAIChat.vcxproj.filters
   └─ main.cpp
```

## 이번 단계에서 배울 C++ 개념

- 함수 선언과 함수 정의
- 매개변수
- 반환값
- 코드 재사용
- 프로그램 구조화
- `main` 함수의 역할 줄이기
- 출력: `std::cout`
- 입력: `std::getline`
- 문자열: `std::string`
- 반복문: `while`
- 조건문: `if`, `else if`, `else`
- 문자열 검색: `std::string::find`
- 검색 실패 확인: `std::string::npos`
- 프로그램 종료 조건: `exit`

이 프로젝트는 `using namespace std;`를 사용하지 않고 `std::` 접두어를 붙입니다. `std::cout`, `std::string`처럼 표준 라이브러리의 이름공간을 명시적으로 적는 방식입니다.

## 함수 역할

- `printWelcomeMessage`: 프로그램 시작 안내 문구를 출력합니다.
- `getUserInput`: `User:` 프롬프트를 출력하고 `std::getline`으로 사용자 입력을 한 줄 받습니다.
- `isExitCommand`: 입력값이 종료 명령인 `exit`인지 확인합니다.
- `getResponse`: 사용자 입력에 포함된 키워드를 검사해 응답 문자열을 만듭니다.
- `printBotResponse`: AI 응답 형식으로 문자열을 출력합니다.
- `main`: 전체 실행 흐름만 관리합니다.

## Visual Studio 2022에서 여는 방법

1. `D:\KM\K82\NA\NA\LocalAIChat.sln` 파일을 Visual Studio 2022로 엽니다.
2. 솔루션 탐색기에서 `LocalAIChat` 프로젝트와 `main.cpp`가 보이는지 확인합니다.

## 빌드 방법

1. 상단 메뉴에서 `빌드 > 솔루션 빌드`를 선택합니다.
2. 또는 `Ctrl + Shift + B`를 누릅니다.

## 실행 방법

1. 상단 메뉴에서 `디버그 > 디버그하지 않고 시작`을 선택합니다.
2. 또는 `Ctrl + F5`를 누릅니다.
3. 콘솔에서 문장을 입력하고 응답을 확인합니다.
4. 종료하려면 `exit`를 정확히 입력합니다.

## 디버그 연습 방법

1. `main.cpp`에서 `getUserInput`, `isExitCommand`, `getResponse`, `printBotResponse` 호출 줄에 중단점을 설정합니다.
2. `F5`로 디버그를 시작합니다.
3. `F10`으로 한 줄씩 실행합니다.
4. `input` 변수에 저장된 사용자 입력을 확인합니다.
5. `getResponse` 함수의 반환값을 확인합니다.
6. `main` 함수가 전체 흐름만 관리하는지 확인합니다.

## 예상 실행 예시

```text
LocalAIChat 2단계 기본 채팅 프로그램
문장을 입력하면 간단한 규칙으로 응답합니다.
종료하려면 exit를 입력하세요.

User: 안녕
AI: 안녕하세요! C++ 채팅 프로그램에 오신 것을 환영합니다.
User: C++ 공부 중이야
AI: C++ 기초를 배우고 있군요. 반복문, 조건문, 함수를 차근차근 익혀봅시다.
User: 도움
AI: 사용할 수 있는 입력 예시는 안녕, C++, 도움, exit입니다.
User: 오늘 뭐 해?
AI: 아직은 간단한 규칙으로만 대답할 수 있어요. '도움'을 입력해보세요.
User: exit
AI: 프로그램을 종료합니다.
```

## 2단계 완료 기준

- 프로그램이 실행됩니다.
- 사용자의 한 줄 입력을 받을 수 있습니다.
- 입력은 `std::getline`으로 처리합니다.
- `exit` 입력 시 프로그램이 종료됩니다.
- `안녕`, `C++`, `도움` 키워드에 반응합니다.
- 그 외 입력에는 기본 응답을 출력합니다.
- 기존 `getResponse` 함수가 유지됩니다.
- `printWelcomeMessage`, `getUserInput`, `isExitCommand`, `printBotResponse` 함수가 추가됩니다.
- `main` 함수는 전체 흐름만 보이도록 단순하게 정리됩니다.
- 여러 `.cpp`/`.h` 파일로 분리하지 않습니다.

## 아직 구현하지 않은 것

- 실제 AI 모델 연결
- Ollama 연결
- OpenAI API 연결
- 서버 또는 HTTP 요청
- 파일 저장
- 대화 기록 저장
- `vector`, `map` 같은 자료구조 사용
- `struct`
- `class`
- 포인터
- 동적 메모리 할당
- 스마트 포인터
- 템플릿
- 람다
- GUI
- 여러 `.cpp`/`.h` 파일 분리

## 다음 단계 안내

3단계 기능은 아직 구현하지 않습니다. 다음 단계에서는 사용자가 요청할 때만 입력 처리나 응답 규칙을 더 정리하는 방식으로 확장합니다.

## 출력 또는 입력 글자가 깨질 때 확인할 사항

- Visual Studio 소스 파일 인코딩이 UTF-8인지 확인합니다.
- Windows 콘솔 인코딩을 확인합니다.
- 터미널 글꼴이 한글을 지원하는지 확인합니다.

## 수동 검증 절차

1. `D:\KM\K82\NA\NA\LocalAIChat.sln` 파일을 Visual Studio 2022로 엽니다.
2. 솔루션 탐색기에서 `LocalAIChat` 프로젝트와 `main.cpp`가 보이는지 확인합니다.
3. 상단 메뉴에서 `빌드 > 솔루션 빌드`를 실행합니다.
4. 오류 없이 빌드되는지 확인합니다.
5. `Ctrl + F5`로 실행합니다.
6. `안녕`, `C++ 공부 중이야`, `도움`, `오늘 뭐 해?`, `exit`를 차례대로 입력해 봅니다.
7. 각 입력에 맞는 응답이 출력되는지 확인합니다.
8. `exit` 입력 후 프로그램이 종료되는지 확인합니다.