# 游戏控制器的Framework层

## 简介

GameController用于给游戏开发者提供游戏外设接入能力的API以及给设备厂商提供输入转触控能力。
其中game_controller_framework为GameController的framework层，用于提供游戏外设上下线监听和外设输入事件监听；
以及给设备厂商提供输入转触控的实现能力。

系统架构图：
![系统架构图](./figures/system_arch.png)
GameControllerFramework内部核心模块：
![代码架构图](./figures/code_arch.png)

- **window**

1) 向窗口注册需要拦截监听的设备并拦截外设的输入事件。

2) 监听窗口大小变化。

- **multi_modal_input**

1) 对接多模输入，监听设备的上线和下线事件。

2) 设备上线时，执行外设识别。

- **key_mapping**

1) 读取配置判断是否需要启动输入转触控功能。

2) 读取按键映射的模板配置。

3) 监听设备按键信息，发送打开编辑模板配置的通知。

4) 基于按键映射模板配置，将输入转为触控指令。

- **gamecontroller_service**

提供GameControllerService的InnerAPI接口。

- **bundle_info**

获取当前应用的信息。

## 目录

```
/domain/game_controller/game_controller_framework
├── frameworks                     # 框架代码
│   ├── capi                       # capi的实现层
│   │   ├── include
│   │   └── src
│   └── native                     # 前后端组件对接层
│       ├── bundle_info            # 包信息查询
│       │   ├── include
│       │   └── src
│       ├── gamecontroller_service # 对接GameController的SA
│       │   ├── include
│       │   └── src
│       ├── key_mapping            # 按键映射的实现
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

## 编译

1. [下载OpenHarmony主干代码](https://www.openharmony.cn/download/)
2. 执行编译

```shell
./build.sh --product-name rk3568 --ccache --build-target game_controller_framework --build-variant root
```

3. 编译结果路径：/out/rk3568/game/game_controller_framework

## 参考文档

[开发指南](https://gitcode.com/weixin_42784160/docs/blob/master/zh-cn/application-dev/game-controller/Readme-CN.md)

[API文档](https://gitcode.com/weixin_42784160/docs/blob/master/zh-cn/application-dev/reference/apis-game-controller-kit/Readme-CN.md)

[输入转触控特性](https://gitcode.com/openharmony-sig/game_game_controller_framework/wiki/%E8%BE%93%E5%85%A5%E8%BD%AC%E8%A7%A6%E6%8E%A7%E7%89%B9%E6%80%A7.md)

## 相关仓

- [Game Controller Service](https://gitcode.com/openharmony-sig/game_game_controller_service)

## 约束

- 语言版本
    - C++11或以上