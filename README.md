# GameController Framework

## Introduction

The GameController is designed to provide game developers with the capability to integrate game peripherals and
to offer terminal device manufacturers the ability to convert input into touch controls.

- The primary functions of the framework layer for GameController include:
    - Providing API interfaces for game developers to implement the monitoring of the connection and disconnection
      of game peripherals, as well as the input monitoring of these peripherals.
    - Enabling terminal device manufacturers to offer gamers the ability to play games using peripherals such as
      keyboards and mouse, without requiring game developers to adapt their software, through the input-to-touch
      conversion feature.
    - Offering InnerApi to terminal device manufacturers for configuring information about
      game peripherals and the input-to-touch conversion settings.
- The primary functions of the service layer for GameController include:
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

- GameController(Framework) is the framework layer for GameController.
- GameController(SA) is the service layer for GameController.

Core Modules of GameControllerFramework:
![Code Architecture Diagram](./figures/code_arch.PNG)

### Framework Layer

- **Window**
    - Registering with the window requires intercepting and listening to input events from game peripherals.
        - If the developer calls the API to listen to input events from game peripherals,
          the intercepted input event callbacks will be notified to the game.
        - If the game is configured to support input-to-touch conversion, the intercepted input events from
          game peripherals will be used for the input-to-touch feature.

    - Monitor window size changes for use in input-to-touch feature.

- **MultiModalInput**

    - Connect to multi-mode-input and monitor the online and offline events of gaming peripherals.

        - If the developer calls the API to monitor the online and offline events of gaming peripherals, the online and
          offline event callbacks of the gaming peripherals will be notified to the game.
        - If the game is configured to support input-to-touch conversion, the corresponding input-to-touch conversion
          configuration for the gaming peripheral type will be loaded.

    - Upon receiving the gaming equipment online, identify the category of the gaming peripherals.

- **KeyMapping**

    - Read the configuration from game_support_key_mapping.json to determine whether to enable the input-to-touch
      functionality.
    - Read the corresponding game peripheral type's touch conversion configuration from the game_controller_service.
    - Based on the input events and categories of gaming peripherals, determine whether to send a notification for
      configuring the input-to-touch configuration.
        - When the keyboard inputs Q, W, or P, it indicates the need to open the input-to-touch configuration
          page for the keyboard.
        - When the floating controller function key is clicked, it indicates the need to open the input-to-touch
          configuration
          configuration interface for the floating controller.

    - Based on the input-to-touch configuration, the input events of gaming peripherals (such as key events, mouse
      events,
      etc.)
      are converted into touchscreen events on the screen.

- **GameControllerService**

  Provide the InnerAPI interface of the game_controller_service to terminal manufacturers for configuring game
  peripheral information and input-to-touch conversion data.

- **BundleInfo**

  Obtain information about the current application for input-to-touch conversion features.

### Service Layer

- **DeviceManager**
    - Perform device category identification for gaming peripherals. Determine whether the device category is a
      keyboard, game pad, etc.
    - Other system SA synchronize gaming peripheral information to the GameControllerService. Other system SA refers to
      system service developed by terminal device manufacturers, such as Game Services Of Terminal Manufacturers
      in the system architecture diagram.
- **Event**
    - Send notifications to other system SA about the online status of gaming peripherals and the opening of the
      configuration editing page for input-to-touch conversion.
    - Notify the current game of the configuration update event for input-to-touch conversion and the switch change
      event for
      input-to-touch conversion.
- **KeyMapping**
    - Other system SA synchronize the list of games that support input-to-touch conversion with the
      GameControllerService.
    - Other system SA synchronize the default and custom configuration for input-to-touch conversion with the
      GameControllerService.

## Launch of GameController(SA)

GameController (SA) is an independent process and not a resident process. The following two scenarios will start SA.it
will be initiated in the following two scenarios.

- Scene 1: When the gaming peripherals are connected, the GameControllerFramework will initiate the SA through samgr (
  System Service Management Component) to identify the device type.
- Scene 2: When the game service of the terminal device manufacturer configures game peripheral information or
  input-to-touch conversion configuration information through the InnerApi of the GameControllerFramework, the
  GameControllerFramework will invoke the SA via samgr (the system service management component).

The InnerAPI involving the SA that is being pulled up is located
in [gamecontroller_server_client.h](https://gitcode.com/openharmony-sig/game_game_controller_framework/blob/master/frameworks/native/gamecontroller_service/include/gamecontroller_server_client.h).

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
│       ├── gamecontroller_service # Connects to the SA of the GameController.
│       ├── key_mapping            # Implementation of Key Mapping 
│       ├── multi_modal_input      # Interconnection with multi-mode input 
│       └── window                 # Interconnection window to game devices's input
├── service                        # Service Layer Code
│   ├── common                     # Provide common methods
│   ├── device_manager             # Device Management
│   ├── event                      # Provides the event processing capability
│   ├── ipc                        # IPC interface implementation layer
│   └── key_mapping                # Input-to-touch configuration management
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
