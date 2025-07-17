# 游戏控制器的Framework层

## 简介

GameController用于给游戏开发者提供游戏外设接入能力的API以及给设备厂商提供输入转触控能力。
其中game_controller_framework为GameController的framework层，用于提供游戏外设上下线监听和外设输入事件监听；以及给设备厂商提供输入转触控的实现能力。

## 目录

```
/domain/game_controller/game_controller_framework
├── frameworks                     # 框架代码
│   ├── capi                       # capi的实现层
│   │   ├── include
│   │   └── src
│   └── native                     # 前后端组件对接层
│       ├── common
│       │   ├── include
│       │   └── src
│       ├── gamecontroller_service # 对接GameController的SA
│       │   ├── include
│       │   └── src
│       ├── input_to_touch         # 输入转触控的能力入口
│       │   ├── include
│       │   └── src
│       ├── key_mapping            # 按键映射的管理
│       │   ├── include
│       │   └── src
│       ├── multi_modal_input      # 对接多模输入实现设备的监听
│       │   ├── include
│       │   └── src
│       └── window                 # 对接窗口实现外设输入的监听
│           ├── include
│           └── src  
├── interfaces                     # 接口存放目录 
│   └── kits                       # 对外接口存放目录 
│       └── c                      # napi接口存放目录
├── test                           # 测试代码
    └── mock                       # mock代码
        └── gamecontroller_service
        └── multi_modal_input
    └── unittest                   # 单元测试  
        └── multi_modal_input 
        └── window   
```

## 约束

- 语言版本
    - C++11或以上