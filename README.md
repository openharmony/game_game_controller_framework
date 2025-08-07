# GameController Framework

## Introduction

The GameController provides game developers with APIs for integrating gaming peripherals and offers device manufacturers
the capability
to convert input into touch controls.
The game_controller_framework is the framework layer of GameController, designed to provide listening capabilities for
the connection
and disconnection of gaming peripherals and the monitoring of input events from these peripherals. It also offers device
manufacturers
the ability to implement input-to-touch conversion.

System Architecture Diagram:
![System Architecture Diagram](./figures/system_arch_en.png)
Core Modules of GameControllerFramework:
![Code Architecture Diagram](./figures/code_arch.png)

- **window**

1) Register the devices that need to be intercepted and monitored at the window,
   and intercept input events from peripherals.

2) Monitor window size and state changes.

- **multi_modal_input**

1) Interface with multi-mode inputs to monitor the online and offline events of devices.
2) When a device goes online, perform device authentication for the peripheral.

- **key_mapping**

1) Read the configuration to determine whether to enable the input-to-touch function.
2) Read the template configuration for key mapping.
3) Monitor the device's key information and send a notification to open the edit template configuration.
4) Based on the key mapping template configuration, convert input into touch commands.

- **gamecontroller_service**

Provide the InnerAPI interface for GameControllerService.

- **bundle_info**

Retrieve information about the current application.

## Directory Structure

```
/domain/game_controller/game_controller_framework
├── frameworks                     # Framework Code
│   ├── capi                       # NAPI Implementation Layer
│   │   ├── include
│   │   └── src
│   └── native                     # Native Code
│       ├── bundle_info            # Bundle Information Inquiry
│       │   ├── include
│       │   └── src
│       ├── gamecontroller_service # Connects to the SA of the GameController.
│       │   ├── include
│       │   └── src
│       ├── key_mapping            # Implementation of Key Mapping 
│       │   ├── include
│       │   └── src
│       ├── multi_modal_input      # Interconnection with multi-mode input 
│       │   ├── include
│       │   └── src
│       └── window                 # Interconnection window to game devices's input
│           ├── include
│           └── src  
├── interfaces                     # Interface Storage Directory 
│   └── kits                        
│       └── c                      # Directory for storing the NAPI interface.
├── test                           # Test Code
    └── mock                       # Mock Code
        └── gamecontroller_service
        └── multi_modal_input
    └── unittest                   # Unit test  
        └── multi_modal_input 
        └── window   
```

## Compile

1. [Download the master branch code of OpenHarmony](https://www.openharmony.cn/download/)
2. Execute compilation

```shell
./build.sh --product-name rk3568 --ccache --build-target game_controller_framework --build-variant root
```

3. The result path of compilation : /out/rk3568/game/game_controller_framework

## Reference Document

[Development Guide](https://gitcode.com/weixin_42784160/docs/blob/master/zh-cn/application-dev/game-controller/Readme-CN.md)

[API Documentation](https://gitcode.com/weixin_42784160/docs/blob/master/zh-cn/application-dev/reference/apis-game-controller-kit/Readme-CN.md)

[Input-To-Touch Feature](https://gitcode.com/openharmony-sig/game_game_controller_framework/wiki/%E8%BE%93%E5%85%A5%E8%BD%AC%E8%A7%A6%E6%8E%A7%E7%89%B9%E6%80%A7.md)

## Related Repository

- [Game Controller Service](https://gitcode.com/openharmony-sig/game_game_controller_service)

## Constraints

- Programming language version
    - C++ 11 or later
