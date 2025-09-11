# Game Controller

## Introduction

GameController provides game developers with an API that enables access to game peripherals, enabling monitoring of both
online and offline game peripherals as well as input events;
Provide InnerAPI to terminal device manufacturers, allowing them to manage identification information for external
devices and configure input-to-touch.

- The primary functions of GameControllerFramework include:
    - Providing API interfaces for game developers to implement the monitoring of the connection and disconnection
      of game peripherals, as well as the input monitoring of these peripherals.
    - Enabling terminal device manufacturers to offer gamers the ability to play games using peripherals such as
      keyboards and mouse, without requiring game developers to adapt their software, through the input-to-touch
      conversion feature.
    - Offering InnerApi to terminal device manufacturers for configuring information about
      game peripherals and the input-to-touch conversion settings.
- The primary functions of GameControllerSA include:
    - Saving game peripheral information to the configuration file `device_config.json`.
    - Saving the list of supported games for input-to-touch conversion to the configuration
      file `game_support_key_mapping.json`.
    - Saving the default input-to-touch conversion configuration for games to the configuration
      file `default_key_mapping.json` and custom configuration to the configuration file `custom_key_mapping.json`.
    - Identifying the categories of game peripherals.
    - Query the default and custom input-to-touch mapping information for the game from default_key_mapping.json and
      custom_key_mapping.json.

System Architecture Diagram:
![System Architecture Diagram](./figures/system_arch_en.PNG)

Note: Currently, only CAPI is available, and ArkTS interface will be planned for the future.

## Core Modules of GameController:

![Code Architecture Diagram](./figures/code_arch_en.PNG)

### GameControllerFramework

- **GameDevice**

  Provide game developers with an API for querying device information and receiving notifications of peripheral device
  connection and disconnection events.

- **GamePad**

  Provide an API for game developers to receive input event notifications from game controllers.
- **InputMonitor**

  Connect to the window Framework and register the input events of the game peripherals that need to be intercepted and
  monitored to the window. Upon receiving the events, perform the following processing:
    - If the developer calls the API to listen to input events from game peripherals,
      the intercepted input event callbacks will be notified to the game.
    - If the game supports input conversion to touch, the intercepted game peripheral input events will be used for the
      input-to-touch feature.

- **DeviceMonitor**

  Connect to multi-modal-input to monitor the online and offline events of gaming peripherals. Upon receiving an
  event, perform the following processing:
    - If the developer calls the API to monitor the online and offline events of gaming peripherals, the online and
      offline event callbacks of the gaming peripherals will be notified to the game.
    - If the game supports input conversion to touch, call InnerAPI to read input-to-touch configuration for
      the corresponding game peripheral type from GameControllerSA.

- **KeyMapping**

  Handle the business logic related to input-to-touch key mapping.
    - Read the configuration from game_support_key_mapping.json to determine whether to enable the input-to-touch
      functionality.
    - Based on the input events and categories of gaming peripherals, determine whether to send a notification for
      configuring the input-to-touch configuration. When pressing Q, W, and P simultaneously on the keyboard, it
      indicates the need to open the input to touch configuration interface of the keyboard.

    - Based on the input-to-touch configuration, the input events of gaming peripherals (such as key events, mouse
      events, etc.) are converted into touchscreen events on the screen.
- **GameControllerClient**

  Definition of InnerAPI for GameControllerSA.

### GameControllerSA

- **DeviceManager**
    - Perform device category identification for gaming peripherals. Determine whether the device category is a
      keyboard, game pad, etc.
    - Other system SA synchronize gaming peripheral information to the GameControllerSA. Other system SA refers to
      system service developed by terminal device manufacturers, such as Game Services Of Terminal Manufacturers
      in the system architecture diagram.

- **KeyMappingManager**
    - Other system SA synchronize the list of games that support input-to-touch conversion with the
      GameControllerSA.
    - Other system SA synchronize the default and custom configuration for input-to-touch conversion with the
      GameControllerSA.

## Launch of GameControllerSA

GameControllerSA is an independent process and not a resident process. The following two scenarios will start SA.it
will be initiated in the following two scenarios.

- Scene 1: When the gaming peripherals are connected, the GameControllerFramework will initiate the SA through samgr (
  System Service Management Component) to identify the device type.
- Scene 2: When the game service of the terminal device manufacturer configures game peripheral information or
  input-to-touch conversion configuration information through the InnerApi of the GameControllerFramework, the
  GameControllerFramework will invoke the SA via samgr (the system service management component).

The InnerAPI involving the SA that is being pulled up is located
in [gamecontroller_server_client.h](https://gitcode.com/openharmony-sig/game_game_controller_framework/blob/master/frameworks/native/sa_client/include/gamecontroller_server_client.h).

## The Framework of the window loads the GameControllerFramework

When the application starts, the framework layer of the window will load the GameControllerFramework's
libgamecontroller_event.z.so through dlopen, thereby enabling the automatic loading of the GameControllerFramework by
the application process upon startup.

## Directory Structure

```
/domain/game/game_controller_framework
├── frameworks                     # Framework Code
│   ├── capi                       # NAPI Implementation Layer
│   │   ├── include
│   │   └── src
│   └── native                     # Native Code
│       ├── bundle_info            # Bundle Information Inquiry
│       ├── sa_client              # Connects to the SA of the GameController.
│       ├── key_mapping            # Implementation of Key Mapping 
│       ├── multi_modal_input      # Interconnection with multi-mode input 
│       └── window                 # Interconnection window to game devices's input
├── service                        # Service Layer Code
│   ├── common                     # Provide common methods
│   ├── device_manager             # Device Management
│   ├── event                      # Provides the event processing capability
│   ├── ipc                        # IPC interface implementation layer
│   └── key_mapping_manager        # Input-to-touch configuration management
├── interfaces                     # Interface Storage Directory 
│   └── kits                        
│       └── c                      # Directory for storing the NAPI interface.
├── test                           # Test Code
    └── mock                       # Mock Code
    └── unittest                   # Unit Test Code 
    └── fuzztest                   # Fuzze Test Code
```

## Compile

1. [Download the master branch code of OpenHarmony](https://www.openharmony.cn/download/)
2. Execute compilation

```shell
./build.sh --product-name rk3568 --ccache --build-target game_controller_framework --build-variant root
```

3. The result path of compilation : /out/rk3568/game/game_controller_framework.

- libgamecontroller_client.z.so
- libgamecontroller_service.z.so
- libgamecontroller_event.z.so
- libohgame_controller.z.so

## Reference Document

[Development Guide](https://gitcode.com/weixin_42784160/docs/blob/master/zh-cn/application-dev/game-controller/Readme-CN.md)

[API Documentation](https://gitcode.com/weixin_42784160/docs/blob/master/zh-cn/application-dev/reference/apis-game-controller-kit/Readme-CN.md)

[Development Guide for Terminal Device Manufacturers Based on GameController's InnerAPI](https://gitcode.com/openharmony-sig/game_game_controller_framework/wiki/Home.md)

[Input-To-Touch Feature](https://gitcode.com/openharmony-sig/game_game_controller_framework/wiki/Home.md)

## Related Repository

- [Window](https://gitee.com/openharmony/window_window_manager/blob/master/README.md)
- [MultiModalInput](https://gitee.com/openharmony/multimodalinput_input/blob/master/README.md)

## Constraints

Programming language version: C++ 11 or later
