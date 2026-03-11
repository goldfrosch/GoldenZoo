# FrogJumpUI Plugin

## Overview

FrogJumpUI is an Unreal Engine plugin for structured UI management. Utilizing the Model-View-Controller (MVC) pattern, it ensures a clear separation of concerns. Its key feature is a stack-based navigation flow, enabling users to traverse UI screens like web pages, easily pushing new views and popping back to the previous one for predictable state management.

## Key Features

### 1. Stack-Based Navigation

- Manage UI screens in a stack, similar to browser back/forward navigation
- Use `PushWidget()` to add new UIs and `PopWidget()` to return to previous ones
- Predictable state management and intuitive UI flow control

### 2. Built-in Object Pooling

- Each UI is identified by a unique Key, and UIs with the same Key reuse their instances
- UI instances remain in memory once created, enabling fast transitions
- Minimizes unnecessary creation/deletion overhead

### 3. Layer Management System

- Provides 4 UI layers:
  - **Gameplay**: HUD elements always visible during gameplay (health bar, minimap, etc.)
  - **Popup**: General additional UIs like equipment windows, settings, etc.
  - **Modal**: Information display modals like notifications, warnings, tutorials
  - **Loading**: Loading screens, the topmost layer
- Automatic layer classification based on Key name prefixes (`Gameplay_`, `Popup_`, `Modal_`, `Loading_`)

### 4. Per-UI Input Management

- Independent Enhanced Input Mapping Context support for each UI
- Input Context automatically added when UI is activated
- Input Context automatically removed when UI is deactivated
- Automatic mouse cursor and Input Mode management

### 5. MVC Architecture

- **Model**: UI data and business logic
- **View**: UI widget (inherits from UUserWidget)
- **Controller**: Input handling and state management for UI

## Installation & Setup

### 1. Enable Plugin

Ensure the FrogJumpUI plugin is enabled in your project's `.uproject` file.

### 2. Project Settings

1. Go to **Edit > Project Settings**
2. Find **Game > UI Settings** section
3. Register UI Keys and their corresponding `BaseUIView` subclasses in the **UI Classes** map

Example:

```
Key: "Gameplay_MainHUD"
Value: BP_MainHUD (Blueprint inheriting from BaseUIView)

Key: "Popup_Inventory"
Value: BP_Inventory (Blueprint inheriting from BaseUIView)
```

### 3. UI Key Naming Convention

Layers are automatically determined by Key name prefixes:

- `Gameplay_*`: Gameplay layer
- `Popup_*`: Popup layer
- `Modal_*`: Modal layer
- `Loading_*`: Loading layer

## Usage

### Basic Usage

#### 1. Get UI Subsystem

```cpp
ULocalPlayerUIManageSubSystem* UISubsystem = GetLocalPlayer()->GetSubsystem<ULocalPlayerUIManageSubSystem>();
if (UISubsystem)
{
    UISubsystem->InitializeUI();
}
```

#### 2. Show UI (Push)

```cpp
UISubsystem->PushWidget(TEXT("Popup_Inventory"));
```

#### 3. Close UI (Pop)

```cpp
UISubsystem->PopWidget();
```

#### 4. Get Current Top UI

```cpp
FString TopUI = UISubsystem->GetTopStackUI();
```

#### 5. Check if UI is in Stack

```cpp
bool bHasInventory = UISubsystem->HasViewUI(TEXT("Popup_Inventory"));
```

#### 6. Reset All UIs

```cpp
UISubsystem->ResetWidget();
```

### Creating Custom UI

#### 1. Create View Class

Create a Blueprint or C++ class that inherits from `BaseUIView`.

```cpp
UCLASS(Blueprintable)
class YOURGAME_API UYourUIView : public UBaseUIView
{
    GENERATED_BODY()

    // UI widget composition
};
```

#### 2. Create Controller Class (Optional)

Inherit from `BaseUIController` to add custom logic.

```cpp
UCLASS(Blueprintable)
class YOURGAME_API UYourUIController : public UBaseUIController
{
    GENERATED_BODY()

public:
    virtual void OnPushUI() override
    {
        // Called when UI is pushed
    }

protected:
    virtual void BindInputAction(UEnhancedInputComponent* InputComponent) override
    {
        // Bind Enhanced Input actions
    }
};
```

#### 3. Create Model Class (Optional)

Inherit from `BaseUIModel` to define data models.

```cpp
UCLASS(Blueprintable)
class YOURGAME_API UYourUIModel : public UBaseUIModel
{
    GENERATED_BODY()

public:
    virtual void InitializeModel() override
    {
        // Initialize model
    }
};
```

#### 4. Connect Controller and Model to View

In Blueprint Editor or C++:

- `ModelClass`: Specify the Model class to use
- `ControllerClass`: Specify the Controller class to use

### Animation Support

`BaseUIView` supports three default animations:

1. **DefaultTickAnimation**: Animation that loops while the UI is displayed
2. **DefaultStartAnimation**: Animation played when UI appears
3. **DefaultEndAnimation**: Animation played when UI disappears

Bind these animations in your widget blueprint and they will play automatically.

### Input Management

#### Enhanced Input Setup

1. In `BaseUIController` subclass:

   - Set `IsInputAccess` to `true`
   - Assign Enhanced Input Mapping Context to `UIMappingContext`

2. Override `BindInputAction()` function to bind Input actions:

```cpp
virtual void BindInputAction(UEnhancedInputComponent* InputComponent) override
{
    Super::BindInputAction(InputComponent);

    // Bind Input actions
    InputComponent->BindAction(YourInputAction, ETriggerEvent::Triggered, this, &UYourUIController::OnInputAction);
}
```

## Architecture

### Class Structure

```
ULocalPlayerUIManageSubSystem
├── UI Stack Management (WidgetStack)
├── Layer Management (WidgetLayers)
└── Controller Instance Management (ControllerInstances)

UBaseUIView
├── UBaseUIModel (Data)
└── UBaseUIController (Logic)

UBaseUIController
├── Input Management
├── Animation Control
└── Input Mode Management
```

### Data Flow

1. `PushWidget(Key)` is called
2. Layer is determined by Key and added to stack
3. If Controller instance doesn't exist, View is created and Controller is initialized
4. Controller's `StartShowUI()` is called
5. `OnPushUI()` is called (can be overridden)
6. Input Context is added and animations play

## Important Notes

1. **Keys must be unique**: Each UI is bound to only one Key.
2. **Layers are determined by prefixes**: Use Key name prefixes correctly.
3. **Call InitializeUI()**: Always call `InitializeUI()` before using the UI subsystem.
4. **Enhanced Input Plugin**: Enhanced Input plugin must be enabled to use Input features.

## Example Scenarios

### Open/Close Inventory UI

```cpp
// Open inventory
UISubsystem->PushWidget(TEXT("Popup_Inventory"));

// Close inventory (go back)
UISubsystem->PopWidget();
```

### Show Modal Dialog

```cpp
// Show modal (displayed above existing UI)
UISubsystem->PushWidget(TEXT("Modal_ConfirmDialog"));

// Close modal
UISubsystem->PopWidget();
```

### Multiple UI Stack Management

```cpp
// Main menu
UISubsystem->PushWidget(TEXT("Popup_MainMenu"));
// Settings menu (displayed above main menu)
UISubsystem->PushWidget(TEXT("Popup_Settings"));
// Confirm dialog (displayed above settings menu)
UISubsystem->PushWidget(TEXT("Modal_Confirm"));

// Close confirm dialog -> return to settings menu
UISubsystem->PopWidget();
// Close settings menu -> return to main menu
UISubsystem->PopWidget();
```

## License

This plugin is created by GoldFrosch.

## Contributing

Bug reports and feature suggestions are welcome on [GitHub](https://github.com/goldfrosch/FrogJumpUI).
