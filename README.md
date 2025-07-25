# GameController Framework

## Introduction

The GameController provides game developers with APIs for integrating gaming peripherals and offers device manufacturers the capability
to convert input into touch controls.
The game_controller_framework is the framework layer of GameController, designed to provide listening capabilities for the connection 
and disconnection of gaming peripherals and the monitoring of input events from these peripherals. It also offers device manufacturers 
the ability to implement input-to-touch conversion.

## Directory Structure

```
/domain/game_controller/game_controller_framework
├── frameworks                     # Framework Code
│   ├── capi                       # NAPI Implementation Layer
│   │   ├── include
│   │   └── src
│   └── native                     # Native Code
│       ├── common
│       │   ├── include
│       │   └── src
│       ├── gamecontroller_service # Connects to the SA of the GameController.
│       │   ├── include
│       │   └── src
│       ├── key_mapping            # Key mapping management 
│       │   ├── include
│       │   └── src
│       ├── input_to_touch         # Entry for the transfer-to-touch capability
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

## Constraints

- Programming language version
    - C++ 11 or later
