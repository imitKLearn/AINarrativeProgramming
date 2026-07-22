# MUD 게임 3단계 - 주요 클래스와 객체 관계 설계

## 1. 설계 목적

이번 3단계의 목적은 텍스트 기반 힐링 MUD 게임을 바로 구현하기 전에, 어떤 클래스가 필요하고 객체들이 서로 어떤 관계를 가져야 하는지 먼저 설계하는 것이다. 2단계의 `MUD_GAME_PLAN.md`에서 정리한 생활, 탐험, 수집, NPC 대화 중심 구조를 C++ 코드로 옮기기 쉬운 형태로 나누는 단계이다.

아직 실제 C++ 코드는 작성하지 않는다. 이번 문서는 나중에 구현할 때 “어떤 객체가 무엇을 소유하고, 어떤 객체는 잠깐 참조만 하는지”를 헷갈리지 않기 위한 설계 기준이다.

C++ 학습 목표와 연결되는 부분은 다음과 같다.

- 클래스가 맡을 역할을 나눈다.
- 객체 생성 시점과 소멸 시점을 생각한다.
- `unique_ptr`, `shared_ptr`, `weak_ptr`, Raw Pointer, 참조의 차이를 비교한다.
- 소유와 참조를 분리해서 댕글링 포인터와 순환 참조를 피한다.
- 작은 콘솔 게임 구조를 나중에 코드로 옮기기 쉽게 준비한다.

소유권 설계가 중요한 이유는 MUD 게임 안에 `Room`, `Item`, `NPC`, `Player`처럼 서로 연결된 객체가 많기 때문이다. 누가 객체를 삭제해야 하는지 명확하지 않으면 같은 객체를 두 번 삭제하거나, 이미 삭제된 객체를 참조하는 문제가 생길 수 있다.

## 2. 주요 클래스 목록과 역할

### 전체 클래스 요약

| 클래스 | 역할 | 한 줄 설명 |
| --- | --- | --- |
| `Game` | 전체 게임 관리 | 게임 시작, 방 생성, 명령 처리, 저장 흐름을 조율한다. |
| `Player` | 플레이어 상태 관리 | 현재 위치와 인벤토리를 가진다. |
| `Room` | 장소 관리 | 장소 설명, 연결 방향, 방 안의 아이템과 NPC를 가진다. |
| `Item` | 아이템 정보 | 이름, 설명, 획득 가능 여부를 가진다. |
| `Inventory` | 소지품 관리 | 플레이어가 가진 아이템 목록을 관리한다. |
| `NPC` | 주민 캐릭터 | 이름, 성격, 기본 대화, 현재 위치 정보를 가진다. |
| `DialogueManager` | 대화 처리 | NPC 대화 요청을 받아 규칙 기반 또는 LLM 응답을 만든다. |
| `CommandParser` | 명령어 분석 | 사용자의 텍스트 입력을 게임 명령으로 해석한다. |
| `GameState` | 저장 가능한 상태 | 위치, 방문 기록, 퀘스트 상태 같은 진행 정보를 모은다. |
| `SaveManager` | 저장/불러오기 | 게임 상태를 텍스트 파일로 저장하거나 불러온다. |
| `LocalLLMClient` | 로컬 LLM 통신 | Ollama 같은 로컬 LLM 요청을 담당한다. |

### Game

| 항목 | 내용 |
| --- | --- |
| 클래스 이름 | `Game` |
| 역할 | 전체 게임 흐름을 관리하는 중심 객체 |
| 주요 데이터 | `Player`, 방 목록, 명령어 처리기, 대화 관리자, 저장 관리자, 게임 상태 |
| 주요 책임 | 게임 초기화, 방 생성, 객체 연결, 입력 루프 실행, 종료 처리 |
| 소유하는 객체 | `Player`, `Room` 목록, `CommandParser`, `DialogueManager`, `SaveManager`, 필요 시 `LocalLLMClient` |
| 참조만 하는 객체 | 현재 명령 처리 중 접근하는 `Room`, `NPC`, `Item` |
| 생성 시점 | 프로그램 시작 후 `main`에서 한 번 생성 |
| 소멸 시점 | 프로그램 종료 시 가장 마지막에 소멸 |

`Game`은 게임 세계의 큰 상자 역할을 한다. 방 전체 목록과 플레이어를 오래 살아 있어야 하는 객체로 관리한다.

### Player

| 항목 | 내용 |
| --- | --- |
| 클래스 이름 | `Player` |
| 역할 | 플레이어 캐릭터의 현재 상태 관리 |
| 주요 데이터 | 이름, 현재 위치, 인벤토리 |
| 주요 책임 | 현재 방 확인, 이동 결과 반영, 인벤토리 접근 |
| 소유하는 객체 | `Inventory` |
| 참조만 하는 객체 | 현재 위치인 `Room` |
| 생성 시점 | `Game` 초기화 중 생성 |
| 소멸 시점 | `Game` 소멸 시 함께 소멸 |

`Player`는 현재 방을 소유하지 않는다. 현재 위치는 이미 `Game`이 소유한 방 중 하나를 가리키는 참조 정보만 가진다.

### Room

| 항목 | 내용 |
| --- | --- |
| 클래스 이름 | `Room` |
| 역할 | MUD의 장소 또는 방 표현 |
| 주요 데이터 | 방 이름, 설명, 연결된 방향, 아이템 목록, NPC 목록, 방문 여부 |
| 주요 책임 | 장소 설명 출력에 필요한 정보 제공, 이동 가능한 방향 관리, 방 안의 아이템과 NPC 관리 |
| 소유하는 객체 | 방 안에 놓인 `Item`, 방에 배치된 `NPC`를 소유할 수 있음 |
| 참조만 하는 객체 | 연결된 다른 `Room` |
| 생성 시점 | `Game` 초기화 중 생성 |
| 소멸 시점 | `Game`이 방 목록을 정리할 때 소멸 |

`Room`은 다른 `Room`을 직접 소유하지 않는다. 방끼리 서로 소유하면 순환 구조가 생기기 쉽기 때문이다.

### Item

| 항목 | 내용 |
| --- | --- |
| 클래스 이름 | `Item` |
| 역할 | 수집 가능한 물건 표현 |
| 주요 데이터 | 이름, 설명, 획득 가능 여부, 사용 가능 여부 |
| 주요 책임 | 아이템 정보 제공, 사용 시 출력할 간단한 설명 제공 |
| 소유하는 객체 | 없음 |
| 참조만 하는 객체 | 없음 |
| 생성 시점 | 방 생성 시 또는 아이템 배치 시 생성 |
| 소멸 시점 | 방이나 인벤토리가 소유권을 잃고 삭제될 때 소멸 |

아이템은 소유권 이동을 배우기 좋은 객체이다. 방에 있던 아이템을 플레이어가 주우면 방에서 인벤토리로 소유권이 이동한다.

### Inventory

| 항목 | 내용 |
| --- | --- |
| 클래스 이름 | `Inventory` |
| 역할 | 플레이어가 가진 아이템 목록 관리 |
| 주요 데이터 | 아이템 목록 |
| 주요 책임 | 아이템 추가, 아이템 제거, 인벤토리 출력, 아이템 검색 |
| 소유하는 객체 | 플레이어가 주운 `Item` |
| 참조만 하는 객체 | 없음 |
| 생성 시점 | `Player` 생성 시 함께 생성 |
| 소멸 시점 | `Player` 소멸 시 함께 소멸 |

초기에는 `Inventory`를 `Player`의 값 멤버로 두는 것이 단순하다. 인벤토리 자체를 다른 객체와 공유할 필요가 없기 때문이다.

### NPC

| 항목 | 내용 |
| --- | --- |
| 클래스 이름 | `NPC` |
| 역할 | 주민 또는 대화 가능한 캐릭터 표현 |
| 주요 데이터 | 이름, 설명, 성격, 기본 대화, 퀘스트 힌트, 현재 위치 정보 |
| 주요 책임 | NPC 정보 제공, 대화에 필요한 성격과 기본 문장 제공 |
| 소유하는 객체 | 없음 또는 아주 간단한 개인 상태 |
| 참조만 하는 객체 | 현재 위치인 `Room`, 대화 처리를 위한 `DialogueManager` |
| 생성 시점 | `Game` 초기화 중 또는 `Room` 생성 중 생성 |
| 소멸 시점 | NPC를 소유한 객체가 소멸할 때 함께 소멸 |

초기 구조에서는 `Room`이 자기 방에 있는 NPC를 소유하는 방식이 이해하기 쉽다. 나중에 NPC가 방을 이동해야 한다면 `Game`이 NPC 전체 목록을 소유하고 `Room`은 참조만 하는 방식으로 바꿀 수 있다.

### DialogueManager

| 항목 | 내용 |
| --- | --- |
| 클래스 이름 | `DialogueManager` |
| 역할 | NPC 대화 응답 생성 담당 |
| 주요 데이터 | 로컬 LLM 클라이언트 참조, fallback 규칙 대화 목록 또는 기본 문장 |
| 주요 책임 | NPC 정보와 플레이어 질문을 받아 응답 생성, 로컬 LLM 실패 시 규칙 기반 응답 반환 |
| 소유하는 객체 | 선택적으로 `LocalLLMClient` |
| 참조만 하는 객체 | `NPC`, `Room`, `Player`, `GameState` |
| 생성 시점 | `Game` 초기화 중 생성 |
| 소멸 시점 | `Game` 소멸 시 함께 소멸 |

`DialogueManager`는 NPC를 소유하지 않는다. 대화에 필요한 정보를 잠깐 받아 응답을 만드는 역할만 한다.

### CommandParser

| 항목 | 내용 |
| --- | --- |
| 클래스 이름 | `CommandParser` |
| 역할 | 사용자 입력 분석 |
| 주요 데이터 | 특별한 상태 없음 또는 마지막 명령어 정보 |
| 주요 책임 | `look`, `north`, `take 사과`, `talk guide` 같은 입력을 명령 종류와 대상 문자열로 나누기 |
| 소유하는 객체 | 없음 |
| 참조만 하는 객체 | 없음 또는 처리 중 필요한 문자열 |
| 생성 시점 | `Game` 초기화 중 생성 |
| 소멸 시점 | `Game` 소멸 시 함께 소멸 |

초기에는 복잡한 파서가 아니라 문자열 비교와 간단한 분리만 담당하게 한다.

### GameState

| 항목 | 내용 |
| --- | --- |
| 클래스 이름 | `GameState` |
| 역할 | 저장 가능한 진행 상태 모음 |
| 주요 데이터 | 현재 방 ID, 방문한 방, 간단한 퀘스트 상태, 획득한 수집품 정보 |
| 주요 책임 | 저장/불러오기에 필요한 상태를 한곳에 모으기 |
| 소유하는 객체 | 없음. 상태 값만 보관 |
| 참조만 하는 객체 | 필요 시 `Player`, `Room` 정보에서 값을 복사해서 저장 |
| 생성 시점 | `Game` 초기화 중 생성 |
| 소멸 시점 | `Game` 소멸 시 함께 소멸 |

`GameState`는 실제 객체를 소유하기보다 저장하기 쉬운 문자열이나 bool 값을 모아두는 쪽이 초보 단계에 적합하다.

### SaveManager

| 항목 | 내용 |
| --- | --- |
| 클래스 이름 | `SaveManager` |
| 역할 | 게임 상태 저장과 불러오기 담당 |
| 주요 데이터 | 저장 파일 이름 |
| 주요 책임 | `GameState`를 텍스트 파일로 저장, 텍스트 파일에서 상태 읽기 |
| 소유하는 객체 | 없음 |
| 참조만 하는 객체 | 저장하거나 불러올 `GameState` |
| 생성 시점 | `Game` 초기화 중 생성 |
| 소멸 시점 | `Game` 소멸 시 함께 소멸 |

`SaveManager`는 게임 객체를 직접 소유하지 않는다. 저장 대상 상태를 받아 파일로 옮기는 도구 역할이다.

### LocalLLMClient

| 항목 | 내용 |
| --- | --- |
| 클래스 이름 | `LocalLLMClient` |
| 역할 | Ollama 같은 로컬 LLM과 통신하는 객체 |
| 주요 데이터 | 서버 주소, 모델 이름, timeout 설정 |
| 주요 책임 | 프롬프트 전송, 응답 문자열 반환, 연결 실패 처리 |
| 소유하는 객체 | 없음 |
| 참조만 하는 객체 | 없음 |
| 생성 시점 | `Game` 또는 `DialogueManager` 초기화 중 생성 |
| 소멸 시점 | 소유한 객체가 소멸할 때 함께 소멸 |

MUD 3단계에서는 실제 연결 코드를 작성하지 않는다. 다만 NPC 대화 확장 시 `DialogueManager`가 `LocalLLMClient`를 사용한다는 구조만 설계한다.

## 3. 클래스 간 관계

| 관계 | 설명 | 소유 여부 | 참조 대상 | 권장 방식 | 선택 이유 | 생성 시점 | 소멸 시점 | 위험 요소 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `Game -> Player` | 게임이 플레이어 하나를 관리한다. | 소유 | `Player` | `unique_ptr<Player>` 또는 값 멤버 | 소유자가 하나라 명확하다. | `Game` 초기화 | `Game` 소멸 | 여러 곳에서 소유하면 삭제 시점이 헷갈린다. |
| `Game -> Room 목록` | 게임 월드의 모든 방을 관리한다. | 소유 | `Room`들 | `vector<unique_ptr<Room>>` | 방 전체 수명은 게임이 관리한다. | `Game` 초기화 | `Game` 소멸 | 방끼리 서로 소유하면 순환 구조가 생긴다. |
| `Game -> CommandParser` | 입력 분석기를 가진다. | 소유 | `CommandParser` | 값 멤버 또는 `unique_ptr` | 공유할 필요가 없다. | `Game` 초기화 | `Game` 소멸 | 불필요한 공유 포인터 사용 |
| `Game -> DialogueManager` | NPC 대화 처리를 맡긴다. | 소유 | `DialogueManager` | `unique_ptr<DialogueManager>` 또는 값 멤버 | 게임 전체에서 하나만 있어도 충분하다. | `Game` 초기화 | `Game` 소멸 | NPC가 직접 소유하면 중복 생성될 수 있다. |
| `Game -> GameState` | 저장 가능한 상태를 관리한다. | 소유 | `GameState` | 값 멤버 | 단순 상태 값이라 값 멤버가 쉽다. | `Game` 초기화 | `Game` 소멸 | 실제 객체 포인터를 저장하면 저장/복원이 어려워진다. |
| `Game -> SaveManager` | 저장 기능을 관리한다. | 소유 | `SaveManager` | 값 멤버 또는 `unique_ptr` | 저장 도구는 하나면 충분하다. | `Game` 초기화 | `Game` 소멸 | 게임 객체를 직접 소유하게 만들면 역할이 커진다. |
| `Player -> Inventory` | 플레이어가 소지품 가방을 가진다. | 소유 | `Inventory` | 값 멤버 권장 | 플레이어와 수명이 같다. | `Player` 생성 | `Player` 소멸 | 별도 동적 할당은 초보 단계에서 불필요하다. |
| `Inventory -> Item` | 인벤토리가 주운 아이템을 가진다. | 소유 | `Item`들 | `vector<unique_ptr<Item>>` | 아이템 소유권 이동 학습에 좋다. | 아이템 줍기 | 아이템 제거 또는 게임 종료 | Raw Pointer면 삭제 책임이 불명확하다. |
| `Player -> 현재 Room` | 플레이어의 현재 위치를 나타낸다. | 참조 | `Room` | `Room*` 또는 `Room&` | 플레이어가 방을 소유하지 않는다. | 플레이어 위치 설정 | 위치 변경 시 다른 방 참조 | `Game`의 방보다 오래 살아 있으면 안 된다. |
| `Room -> 연결된 Room` | 북/남/동/서 이동 관계를 가진다. | 참조 | 다른 `Room` | `Room*` 또는 roomId | 방끼리 소유하지 않는다. | 방 연결 설정 | 게임 종료 | 잘못된 포인터, 삭제된 방 참조 |
| `Room -> 방 안 Item` | 바닥에 놓인 아이템을 관리한다. | 소유 | `Item`들 | `vector<unique_ptr<Item>>` | 플레이어가 줍기 전에는 방이 소유한다. | 방 생성 또는 아이템 배치 | 줍거나 게임 종료 | 인벤토리 이동 시 `move` 필요 |
| `Room -> 방 안 NPC` | 방에 있는 주민을 관리한다. | 소유 또는 참조 | `NPC`들 | 초기에는 `vector<NPC>` 또는 `vector<unique_ptr<NPC>>` | NPC 이동이 없으면 방 소유가 쉽다. | 방 생성 | 방 소멸 | NPC가 이동하면 구조 변경 필요 |
| `NPC -> 현재 Room` | NPC가 있는 장소 정보 | 참조 | `Room` | `Room*` 또는 roomId | NPC가 방을 소유하지 않는다. | NPC 배치 | NPC 이동 또는 게임 종료 | Room과 NPC가 서로 `shared_ptr`를 가지면 순환 참조 위험 |
| `DialogueManager -> LocalLLMClient` | 로컬 LLM 요청을 수행한다. | 소유 또는 참조 | `LocalLLMClient` | `unique_ptr<LocalLLMClient>` 또는 참조 | 클라이언트 하나를 재사용할 수 있다. | 대화 관리자 생성 | 대화 관리자 소멸 | NPC마다 클라이언트를 만들면 중복이 많다. |
| `DialogueManager -> NPC/Room/Player` | 대화 생성에 필요한 정보 접근 | 참조 | `NPC`, `Room`, `Player` | 참조 또는 const 참조 | 대화 생성 중 잠깐 읽기만 한다. | 대화 요청 시 | 함수 종료 | 소유로 바꾸면 역할이 무거워진다. |

## 4. 소유권 방식 선택 기준

C++에서 “소유한다”는 것은 그 객체를 누가 만들고, 누가 책임지고 없앨지를 정한다는 뜻이다. “참조한다”는 것은 객체를 없앨 책임 없이 잠깐 사용한다는 뜻이다.

| 방식 | 의미 | 언제 사용? | 주의점 |
| --- | --- | --- | --- |
| `unique_ptr` | 객체를 단 하나의 소유자가 가진다. | Game이 Room을 소유하거나 Inventory가 Item을 소유할 때 | 복사할 수 없고 이동해야 한다. |
| `shared_ptr` | 여러 객체가 하나의 객체를 공동 소유한다. | 정말 여러 소유자가 필요할 때만 | 순환 참조가 생기기 쉽다. |
| `weak_ptr` | `shared_ptr` 대상을 소유하지 않고 관찰한다. | shared_ptr 순환 참조를 끊어야 할 때 | 초보 단계에서는 되도록 사용하지 않는다. |
| Raw Pointer | 소유하지 않고 주소만 가리킨다. | Game이 소유한 Room을 Player가 현재 위치로 가리킬 때 | 대상이 먼저 삭제되면 위험하다. |
| 참조 | 반드시 존재하는 객체를 잠깐 사용한다. | 함수에서 NPC나 Room 정보를 읽을 때 | null을 표현할 수 없다. |

검토 질문:

- 이 객체를 정말 소유해야 하는가?
- 소유권이 하나인가, 여러 개인가?
- 단순히 잠깐 읽기만 하면 되는가?
- 참조 대상이 먼저 소멸할 가능성이 있는가?
- `shared_ptr`를 습관적으로 쓰고 있지는 않은가?

기본 원칙은 `unique_ptr` 중심이다. 공동 소유가 분명하지 않다면 `shared_ptr`를 사용하지 않는다. 접근만 필요하면 Raw Pointer나 참조를 사용하되, 그 대상이 더 오래 살아 있다는 조건을 문서와 코드 구조로 보장해야 한다.

## 5. 추천 소유권 구조

### Game 중심 소유 구조

`Game`은 전체 세계를 오래 보관하는 객체이므로, 가장 많은 객체를 소유한다.

| 소유자 | 대상 | 추천 방식 | 이유 |
| --- | --- | --- | --- |
| `Game` | `Player` | `unique_ptr<Player>` 또는 값 멤버 | 플레이어는 하나이고 게임이 소유한다. |
| `Game` | `Room` 목록 | `vector<unique_ptr<Room>>` | 모든 방의 수명을 Game이 책임진다. |
| `Game` | `CommandParser` | 값 멤버 또는 `unique_ptr` | 입력 분석기는 공유할 필요가 없다. |
| `Game` | `DialogueManager` | `unique_ptr<DialogueManager>` 또는 값 멤버 | 대화 처리를 한곳에 모은다. |
| `Game` | `SaveManager` | 값 멤버 또는 `unique_ptr` | 저장 도구는 게임에 속한다. |
| `Game` | `GameState` | 값 멤버 | 단순 상태 값은 동적 할당이 필요 없다. |

### Room, Item, NPC 구조

| 상황 | 추천 방식 | 이유 |
| --- | --- | --- |
| Room이 다른 Room과 연결됨 | `Room* north`, `Room* south` 또는 roomId | 연결은 소유가 아니라 이동 경로 참조다. |
| Room 안에 Item이 놓임 | `vector<unique_ptr<Item>> items` | 방이 바닥 아이템을 소유한다. |
| Player가 Item을 줍는다 | `unique_ptr<Item>`을 `move`해서 Inventory로 이동 | 소유권 이동을 명확히 배울 수 있다. |
| Room 안에 NPC를 둔다 | 초기에는 `vector<NPC>` 또는 `vector<unique_ptr<NPC>>` | NPC 이동이 없으면 방 소유가 단순하다. |
| NPC가 여러 방을 이동한다 | Game이 NPC를 소유하고 Room은 `NPC*` 참조 | 이동하는 NPC는 방 소유보다 중앙 관리가 안전하다. |

### DialogueManager와 LocalLLMClient

`DialogueManager`가 `LocalLLMClient`를 사용해 AI NPC 응답을 만든다. 여러 NPC가 각자 LLM 클라이언트를 소유하지 않게 한다.

추천 방향:

- `Game`이 `LocalLLMClient`를 하나 소유하고 `DialogueManager`가 참조한다.
- 또는 `DialogueManager`가 `unique_ptr<LocalLLMClient>`로 하나 소유한다.
- `NPC`는 LLM 클라이언트를 직접 소유하지 않는다.

초기 구현에서는 `DialogueManager`가 LLM 요청과 fallback 규칙 대화를 모두 담당하는 구조가 가장 읽기 쉽다.

## 6. 생성과 소멸 흐름

### 생성 흐름

| 순서 | 흐름 | 설명 |
| --- | --- | --- |
| 1 | 프로그램 시작 | `main`에서 `Game` 객체를 만든다. |
| 2 | `Game` 생성 | 게임 전체를 초기화한다. |
| 3 | `Player` 생성 | 플레이어 이름과 인벤토리를 준비한다. |
| 4 | `Room` 생성 | 마을 광장, 숲길, 해변 같은 방을 만든다. |
| 5 | `Room` 연결 설정 | 각 방의 north/south/east/west 참조를 설정한다. |
| 6 | `Item` 생성 | 각 방에 사과, 조개껍데기 같은 아이템을 배치한다. |
| 7 | `NPC` 생성 | 마을 안내자 같은 주민을 방에 배치한다. |
| 8 | 보조 관리자 생성 | `CommandParser`, `DialogueManager`, `SaveManager`를 준비한다. |
| 9 | 게임 루프 실행 | 플레이어 입력을 받고 명령을 처리한다. |

### 실행 중 흐름

| 상황 | 흐름 |
| --- | --- |
| 플레이어 이동 | 입력 분석 -> 현재 Room 연결 확인 -> Player의 currentRoom 변경 -> 새 Room 설명 출력 |
| 아이템 줍기 | 현재 Room의 items에서 대상 검색 -> `unique_ptr<Item>`을 Inventory로 이동 -> Room 목록에서 제거 |
| NPC 대화 | 현재 Room의 NPC 검색 -> DialogueManager에 NPC/Room/Player 상태 전달 -> 응답 출력 |

### 소멸 흐름

| 순서 | 흐름 | 설명 |
| --- | --- | --- |
| 1 | 게임 종료 명령 | `quit` 또는 종료 조건이 들어온다. |
| 2 | 저장 처리 | 필요하면 `SaveManager`가 `GameState`를 저장한다. |
| 3 | `Game` 소멸 | Game이 소유한 객체들이 정리된다. |
| 4 | `Room` 목록 소멸 | `vector<unique_ptr<Room>>`가 각 Room을 자동 삭제한다. |
| 5 | Room 안 객체 소멸 | Room이 소유한 Item, NPC가 함께 정리된다. |
| 6 | Player 소멸 | Inventory와 그 안의 Item이 함께 정리된다. |

`unique_ptr`를 사용하면 소유자가 사라질 때 대상 객체도 자동으로 정리되므로, 초보자가 `delete`를 직접 관리할 일이 줄어든다.

## 7. 아이템 소유권 이동 설계

아이템은 MUD 게임에서 소유권을 배우기 좋은 예시이다.

### Room에 떨어진 Item

처음에는 방이 아이템을 소유한다.

- 숲길 Room이 사과를 소유한다.
- 해변 Room이 조개껍데기를 소유한다.
- 플레이어는 아직 그 아이템을 소유하지 않는다.

### Player가 Item을 줍는 순간

플레이어가 `take apple`을 입력하면 다음 일이 일어난다.

1. 현재 Room의 아이템 목록에서 사과를 찾는다.
2. Room의 `vector<unique_ptr<Item>>` 안에 있는 사과 소유권을 꺼낸다.
3. 그 `unique_ptr<Item>`을 Inventory의 아이템 목록으로 이동한다.
4. Room의 아이템 목록에서는 사과가 사라진다.

이때 `unique_ptr`는 복사할 수 없고 이동만 할 수 있다. 그래서 나중에 코드로 옮길 때는 `move`가 필요하다. 이것은 “아이템은 동시에 방과 인벤토리에 소유될 수 없다”는 게임 규칙과도 잘 맞는다.

### Raw Pointer로 처리할 때 문제

Raw Pointer만 사용하면 누가 `delete`해야 하는지 불분명해진다. 방도 아이템을 가리키고 인벤토리도 같은 아이템을 가리키면, 둘 중 누가 삭제해야 할지 애매하다. 잘못하면 이미 삭제된 아이템을 다시 사용하거나 같은 아이템을 두 번 삭제할 수 있다.

### shared_ptr로 처리할 때 문제

`shared_ptr`를 쓰면 방과 인벤토리가 같은 아이템을 동시에 소유할 수 있다. 하지만 게임 규칙상 아이템은 방에 있거나 인벤토리에 있거나 둘 중 하나여야 한다. 공동 소유는 오히려 “어디에 있는 아이템인가?”를 흐리게 만들 수 있다.

따라서 아이템은 `unique_ptr` 기반 소유권 이동이 가장 학습 목적에 잘 맞는다.

## 8. 방 연결 구조 설계

`Room`은 다른 `Room`과 연결되어 플레이어 이동 경로를 만든다. 하지만 연결은 소유가 아니다.

### Room이 다른 Room을 소유하면 안 되는 이유

마을 광장이 숲길을 소유하고, 숲길이 다시 마을 광장을 소유하면 서로가 서로를 가진 구조가 된다. 이런 구조는 소멸 순서를 복잡하게 만들고 순환 구조를 만들 수 있다.

### 권장 구조

- `Game`이 모든 Room을 소유한다.
- 각 `Room`은 다른 Room을 `Room*`로 참조하거나 roomId로 연결한다.
- `Player`의 현재 위치도 `Room*` 또는 roomId로 표현한다.

이 구조에서는 방의 실제 수명은 `Game`이 관리한다. `Room*`는 “저쪽 방으로 갈 수 있다”는 길 안내판처럼 사용한다.

### Room* 사용 조건

`Room*`를 사용하려면 `Game`이 모든 Room보다 오래 살아 있어야 한다. 즉, Room들을 삭제하기 전에 Player나 다른 Room이 그 포인터를 사용하면 안 된다. 이 프로젝트에서는 `Game`이 게임 종료까지 방 목록을 유지하므로 초기 단계에서는 `Room*` 사용이 가능하다.

### roomId 기반 연결 가능성

나중에 안정성을 더 높이고 저장/불러오기를 쉽게 하려면 roomId 기반 연결을 사용할 수 있다.

예시 개념:

- 마을 광장 ID: `town_square`
- 숲길 ID: `forest_path`
- 해변 ID: `beach`

이 방식은 포인터보다 저장하기 쉽고, 파일에서 불러온 뒤 다시 연결하기도 쉽다. 다만 처음 구현에서는 `Room*`가 객체 관계를 눈으로 이해하기 더 좋다.

## 9. NPC와 Local LLM 관계

AI NPC 응답은 자연스러운 대화를 만들 수 있지만, NPC가 로컬 LLM 클라이언트를 직접 소유하는 구조는 추천하지 않는다.

### NPC가 LocalLLMClient를 직접 소유하지 않는 이유

- NPC마다 LLM 클라이언트를 만들면 중복이 많다.
- NPC의 역할이 “캐릭터 정보 보관”이 아니라 “HTTP 통신”까지 커진다.
- 나중에 LLM 설정을 바꿀 때 모든 NPC를 수정해야 할 수 있다.

### 권장 구조

`DialogueManager`가 NPC 대화 생성을 담당한다.

1. 플레이어가 `talk guide`를 입력한다.
2. `Game` 또는 `CommandParser`가 현재 Room에서 NPC를 찾는다.
3. `DialogueManager`가 NPC 성격, 현재 Room 설명, 플레이어 인벤토리, 퀘스트 상태를 모아 프롬프트를 만든다.
4. `DialogueManager`가 `LocalLLMClient`를 사용해 응답을 요청한다.
5. 실패하면 NPC의 기본 규칙 기반 대화를 출력한다.

### 프롬프트에 넣을 정보

- NPC 이름
- NPC 성격
- 현재 방 이름과 설명
- 플레이어가 가진 주요 아이템
- 진행 중인 부탁 상태
- 답변 제한 규칙
- 원본 게임의 고유 대사와 설정을 복제하지 말라는 조건

`NPC`는 자기 성격과 기본 대화를 제공하고, 실제 요청은 `DialogueManager`가 처리하는 구조가 가장 단순하다.

## 10. 순환 참조와 댕글링 포인터 위험 분석

### Room끼리 shared_ptr로 소유할 때 생기는 순환 참조

마을 광장이 숲길을 `shared_ptr`로 소유하고, 숲길이 다시 마을 광장을 `shared_ptr`로 소유하면 서로의 참조 횟수가 줄지 않아 객체가 삭제되지 않을 수 있다. 이것이 순환 참조 문제이다.

해결 방향:

- 방 전체는 `Game`이 소유한다.
- 방끼리는 소유하지 않고 `Room*` 또는 roomId로 연결한다.

### Player가 Room을 소유하면 생기는 문제

Player가 현재 Room을 소유하면 플레이어가 이동할 때 방 소유권도 이동해야 하는 이상한 구조가 된다. 방은 플레이어보다 큰 세계의 일부이므로 `Game`이 소유하고 Player는 현재 위치만 참조해야 한다.

### NPC와 Room이 서로 shared_ptr를 가질 때 문제

Room이 NPC를 `shared_ptr`로 가지고, NPC가 다시 Room을 `shared_ptr`로 가지면 순환 참조가 생길 수 있다. 초기에는 Room이 NPC를 소유하고 NPC는 Room을 포인터 또는 roomId로만 참조하는 방식이 더 단순하다.

### Item을 Raw Pointer로 여러 곳에서 들고 있을 때 문제

Room과 Inventory가 같은 Item Raw Pointer를 들고 있으면 소유자가 불명확하다. 삭제 책임이 애매해서 메모리 누수나 중복 삭제 위험이 있다. Item은 한 곳에서만 소유해야 하며, 방에서 인벤토리로 이동하는 구조가 좋다.

### Room* 사용 시 댕글링 포인터 위험

`Room*`는 대상 Room이 먼저 삭제되면 위험하다. 그래서 아래 조건을 지켜야 한다.

- 모든 Room은 `Game`이 소유한다.
- `Game`은 게임 종료 전까지 Room을 삭제하지 않는다.
- Player와 Room의 `Room*`는 Game이 가진 Room만 가리킨다.

### weak_ptr가 필요한 경우와 필요하지 않은 경우

`weak_ptr`는 `shared_ptr` 순환 참조를 끊을 때 필요하다. 하지만 이번 설계는 `shared_ptr`를 기본으로 쓰지 않으므로 `weak_ptr`도 거의 필요하지 않다. 초보 단계에서는 `unique_ptr`와 소유하지 않는 포인터/참조를 먼저 익히는 편이 좋다.

## 11. 현재 단계에서 가장 추천하는 구조

아래 구조는 확정 코드가 아니라 현재 단계에서 가장 단순하고 학습에 적합한 추천안이다.

### Game

- `unique_ptr<Player> player`
- `vector<unique_ptr<Room>> rooms`
- `unique_ptr<CommandParser> commandParser`
- `unique_ptr<DialogueManager> dialogueManager`
- `unique_ptr<SaveManager> saveManager`
- `GameState gameState`

선택 이유:

- Game이 전체 세계의 소유자임을 분명히 한다.
- Player와 Room의 수명이 Game보다 길 필요가 없다.
- 관리자 객체들도 Game과 함께 생성되고 함께 사라지면 된다.

### Player

- `Inventory inventory`
- `Room* currentRoom`

선택 이유:

- Inventory는 Player의 일부라 값 멤버가 단순하다.
- 현재 위치는 소유가 아니라 참조이므로 `Room*`가 적합하다.

### Room

- `string id`
- `string name`
- `string description`
- `vector<unique_ptr<Item>> items`
- `vector<NPC> npcs` 또는 `vector<unique_ptr<NPC>> npcs`
- `Room* north`
- `Room* south`
- `Room* east`
- `Room* west`

선택 이유:

- Room이 바닥 아이템을 소유하면 아이템 이동이 명확하다.
- NPC가 아직 이동하지 않는다면 `vector<NPC>`가 가장 단순하다.
- Room 연결은 소유가 아니므로 포인터로 표현한다.

### Inventory

- `vector<unique_ptr<Item>> items`

선택 이유:

- 플레이어가 주운 아이템의 소유권을 Inventory가 가진다.
- Room에서 Inventory로 `unique_ptr`를 이동시키는 방식으로 소유권 이동을 배울 수 있다.

### Item

- `string name`
- `string description`
- `bool canTake`
- `bool canUse`

선택 이유:

- 초기에는 아이템 효과가 복잡하지 않으므로 데이터 중심으로 둔다.

### NPC

- `string name`
- `string description`
- `string personality`
- `string defaultDialogue`
- `Room* currentRoom` 또는 `string roomId`

선택 이유:

- NPC는 대화에 필요한 정보를 제공한다.
- LLM 요청이나 게임 상태 변경은 직접 담당하지 않는다.

### DialogueManager

- `LocalLLMClient* localLLMClient` 또는 참조
- 규칙 기반 fallback 대화 처리 함수

선택 이유:

- DialogueManager는 LLM 클라이언트를 사용하지만 꼭 소유할 필요는 없다.
- LLM 실패 시 기본 대화를 반환하는 책임을 이곳에 둔다.

### LocalLLMClient

- `string baseUrl`
- `string modelName`
- 응답 요청 함수 개념

선택 이유:

- 실제 통신 세부사항을 NPC나 Game에서 분리한다.
- MUD 3단계에서는 코드 작성 없이 구조만 설계한다.

### CommandParser

- 입력 문자열 분석 함수 개념
- 명령어 종류와 대상 문자열 분리 개념

선택 이유:

- Game의 입력 루프가 너무 복잡해지지 않게 한다.

### SaveManager

- 저장 파일 이름
- 저장 함수 개념
- 불러오기 함수 개념

선택 이유:

- 저장 방식이 바뀌어도 Game 전체 구조가 크게 바뀌지 않는다.

## 12. 4단계에서 할 일 제안

아직 코드는 작성하지 않는다. 다음 단계에서 구현할 최소 작업은 아래 정도로 제한하는 것이 좋다.

1. Room 3개를 만드는 최소 구조 구현
   - 마을 광장
   - 숲길
   - 해변
2. Player 현재 위치 저장
3. `look` 명령어로 현재 방 정보 출력
4. `north`, `south`, `east`, `west` 이동
5. 이동 실패 메시지 출력
6. `help` 명령어 출력
7. `quit` 명령어로 종료
8. Item은 아직 최소 구조만 추가
   - 사과
   - 조개껍데기
9. NPC는 아직 LLM 연결 없이 1명만 추가
   - 마을 안내자
10. `talk guide` 입력 시 규칙 기반 짧은 안내 출력

4단계에서도 아직 하지 않을 일:

- 실제 AI NPC 연결 코드
- 네트워크 코드
- 파일 저장
- 복잡한 퀘스트 시스템
- 전투 시스템
- 스킬 또는 직업 시스템
- 상점, 박물관, 계절, 날씨
- GUI

4단계의 성공 기준은 “플레이어가 세 개의 방 사이를 이동하고, `look`으로 현재 장소를 확인할 수 있다” 정도로 작게 잡는 것이 좋다.