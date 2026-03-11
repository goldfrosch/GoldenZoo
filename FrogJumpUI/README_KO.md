# FrogJumpUI 플러그인

## 개요

FrogJumpUI는 언리얼 엔진을 위한 구조화된 UI 관리 플러그인입니다. Model-View-Controller (MVC) 패턴을 활용하여 관심사의 명확한 분리를 보장하며, 웹사이트처럼 스택 기반으로 UI 화면을 탐색할 수 있는 핵심 기능을 제공합니다.

## 주요 기능

### 1. 스택 기반 네비게이션

- 웹 브라우저의 뒤로 가기처럼 UI 화면을 스택으로 관리
- `PushWidget()`으로 새로운 UI를 추가하고, `PopWidget()`으로 이전 UI로 복귀
- 예측 가능한 상태 관리와 직관적인 UI 흐름 제어

### 2. 자체 오브젝트 풀링

- 각 UI는 고유한 Key로 식별되며, 동일한 Key의 UI는 인스턴스를 재사용
- UI 인스턴스는 한 번 생성되면 메모리에 유지되어 빠른 전환 가능
- 불필요한 생성/삭제 오버헤드 최소화

### 3. 레이어 관리 시스템

- 4가지 UI 레이어 제공:
  - **Gameplay**: 게임플레이 중 항상 표시되는 HUD (체력바, 미니맵 등)
  - **Popup**: 장비 창, 설정 창 등 일반적인 추가 UI
  - **Modal**: 알림, 경고, 튜토리얼 등 정보 표시용 모달
  - **Loading**: 로딩 화면, 가장 최상위 레이어
- Key 이름의 접두사로 자동 레이어 분류 (`Gameplay_`, `Popup_`, `Modal_`, `Loading_`)

### 4. UI 단위 Input 관리

- 각 UI별로 독립적인 Enhanced Input Mapping Context 지원
- UI가 활성화되면 자동으로 해당 Input Context 추가
- UI가 비활성화되면 Input Context 자동 제거
- 마우스 커서 및 Input Mode 자동 관리

### 5. MVC 아키텍처

- **Model**: UI의 데이터와 비즈니스 로직
- **View**: UI 위젯 (UUserWidget 상속)
- **Controller**: UI의 입력 처리 및 상태 관리

## 설치 및 설정

### 1. 플러그인 활성화

프로젝트의 `.uproject` 파일에서 FrogJumpUI 플러그인이 활성화되어 있는지 확인하세요.

### 2. 프로젝트 설정

1. **Edit > Project Settings**로 이동
2. **Game > UI Settings** 섹션 찾기
3. **UI Classes** 맵에 UI Key와 해당 `BaseUIView` 서브클래스를 등록

예시:

```
Key: "Gameplay_MainHUD"
Value: BP_MainHUD (BaseUIView를 상속한 블루프린트)

Key: "Popup_Inventory"
Value: BP_Inventory (BaseUIView를 상속한 블루프린트)
```

### 3. UI Key 네이밍 규칙

레이어는 Key 이름의 접두사로 자동 결정됩니다:

- `Gameplay_*`: Gameplay 레이어
- `Popup_*`: Popup 레이어
- `Modal_*`: Modal 레이어
- `Loading_*`: Loading 레이어

## 사용법

### 기본 사용

#### 1. UI 서브시스템 가져오기

```cpp
ULocalPlayerUIManageSubSystem* UISubsystem = GetLocalPlayer()->GetSubsystem<ULocalPlayerUIManageSubSystem>();
if (UISubsystem)
{
    UISubsystem->InitializeUI();
}
```

#### 2. UI 표시 (Push)

```cpp
UISubsystem->PushWidget(TEXT("Popup_Inventory"));
```

#### 3. UI 닫기 (Pop)

```cpp
UISubsystem->PopWidget();
```

#### 4. 현재 최상위 UI 확인

```cpp
FString TopUI = UISubsystem->GetTopStackUI();
```

#### 5. 특정 UI가 스택에 있는지 확인

```cpp
bool bHasInventory = UISubsystem->HasViewUI(TEXT("Popup_Inventory"));
```

#### 6. 모든 UI 초기화

```cpp
UISubsystem->ResetWidget();
```

### 커스텀 UI 생성

#### 1. View 클래스 생성

`BaseUIView`를 상속하는 블루프린트 또는 C++ 클래스를 생성합니다.

```cpp
UCLASS(Blueprintable)
class YOURGAME_API UYourUIView : public UBaseUIView
{
    GENERATED_BODY()

    // UI 위젯 구성
};
```

#### 2. Controller 클래스 생성 (선택사항)

`BaseUIController`를 상속하여 커스텀 로직을 추가할 수 있습니다.

```cpp
UCLASS(Blueprintable)
class YOURGAME_API UYourUIController : public UBaseUIController
{
    GENERATED_BODY()

public:
    virtual void OnPushUI() override
    {
        // UI가 Push될 때 호출
    }

protected:
    virtual void BindInputAction(UEnhancedInputComponent* InputComponent) override
    {
        // Enhanced Input 액션 바인딩
    }
};
```

#### 3. Model 클래스 생성 (선택사항)

`BaseUIModel`을 상속하여 데이터 모델을 정의할 수 있습니다.

```cpp
UCLASS(Blueprintable)
class YOURGAME_API UYourUIModel : public UBaseUIModel
{
    GENERATED_BODY()

public:
    virtual void InitializeModel() override
    {
        // 모델 초기화
    }
};
```

#### 4. View에 Controller와 Model 연결

블루프린트 에디터 또는 C++에서:

- `ModelClass`: 사용할 Model 클래스 지정
- `ControllerClass`: 사용할 Controller 클래스 지정

### 애니메이션 지원

`BaseUIView`는 세 가지 기본 애니메이션을 지원합니다:

1. **DefaultTickAnimation**: UI가 표시되는 동안 반복 재생되는 애니메이션
2. **DefaultStartAnimation**: UI가 나타날 때 재생되는 애니메이션
3. **DefaultEndAnimation**: UI가 사라질 때 재생되는 애니메이션

위젯 블루프린트에서 이 애니메이션들을 바인딩하면 자동으로 재생됩니다.

### Input 관리

#### Enhanced Input 설정

1. `BaseUIController` 서브클래스에서:

   - `IsInputAccess`를 `true`로 설정
   - `UIMappingContext`에 Enhanced Input Mapping Context 할당

2. `BindInputAction()` 함수를 오버라이드하여 Input 액션 바인딩:

```cpp
virtual void BindInputAction(UEnhancedInputComponent* InputComponent) override
{
    Super::BindInputAction(InputComponent);

    // Input 액션 바인딩
    InputComponent->BindAction(YourInputAction, ETriggerEvent::Triggered, this, &UYourUIController::OnInputAction);
}
```

## 아키텍처

### 클래스 구조

```
ULocalPlayerUIManageSubSystem
├── UI 스택 관리 (WidgetStack)
├── 레이어 관리 (WidgetLayers)
└── Controller 인스턴스 관리 (ControllerInstances)

UBaseUIView
├── UBaseUIModel (데이터)
└── UBaseUIController (로직)

UBaseUIController
├── Input 관리
├── 애니메이션 제어
└── Input Mode 관리
```

### 데이터 흐름

1. `PushWidget(Key)` 호출
2. Key로 레이어 확인 및 스택에 추가
3. Controller 인스턴스가 없으면 View 생성 및 Controller 초기화
4. Controller의 `StartShowUI()` 호출
5. `OnPushUI()` 호출 (오버라이드 가능)
6. Input Context 추가 및 애니메이션 재생

## 주의사항

1. **Key는 고유해야 합니다**: 각 UI는 하나의 Key에만 바인딩됩니다.
2. **레이어는 접두사로 결정됩니다**: Key 이름의 접두사를 올바르게 사용하세요.
3. **InitializeUI() 호출**: UI 서브시스템 사용 전에 반드시 `InitializeUI()`를 호출해야 합니다.
4. **Enhanced Input 플러그인**: Input 기능을 사용하려면 Enhanced Input 플러그인이 활성화되어 있어야 합니다.

## 예제 시나리오

### 인벤토리 UI 열기/닫기

```cpp
// 인벤토리 열기
UISubsystem->PushWidget(TEXT("Popup_Inventory"));

// 인벤토리 닫기 (뒤로 가기)
UISubsystem->PopWidget();
```

### 모달 다이얼로그 표시

```cpp
// 모달 표시 (기존 UI 위에 표시됨)
UISubsystem->PushWidget(TEXT("Modal_ConfirmDialog"));

// 모달 닫기
UISubsystem->PopWidget();
```

### 여러 UI 스택 관리

```cpp
// 메인 메뉴
UISubsystem->PushWidget(TEXT("Popup_MainMenu"));
// 설정 메뉴 (메인 메뉴 위에 표시)
UISubsystem->PushWidget(TEXT("Popup_Settings"));
// 확인 다이얼로그 (설정 메뉴 위에 표시)
UISubsystem->PushWidget(TEXT("Modal_Confirm"));

// 확인 다이얼로그 닫기 -> 설정 메뉴로 복귀
UISubsystem->PopWidget();
// 설정 메뉴 닫기 -> 메인 메뉴로 복귀
UISubsystem->PopWidget();
```

## 라이선스

이 플러그인은 GoldFrosch에 의해 제작되었습니다.

## 기여

버그 리포트 및 기능 제안은 [GitHub](https://github.com/goldfrosch/FrogJumpUI)에서 환영합니다.
