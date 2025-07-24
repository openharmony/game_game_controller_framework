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

## Constraints

- Programming language version
    - C++ 11 or later
