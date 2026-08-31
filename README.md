# UNEDC_25_E_Code

2025 年全国大学生电子设计竞赛 E 题省级一等奖工程代码。

本仓库记录了从方案搭建、循迹实现到整车联调的历史开发过程，主要包含：

- 基于 **MSPM0G3507** 的小车循迹控制；
- 基于 **STM32H723ZGT6 + HAL** 的步进云台与执行机构控制；
- 多个比赛期间形成的阶段版本与调试工程。

> [!IMPORTANT]
> 这是按比赛现场状态整理的历史工程，不是重新搭建的现代化工程模板。为保证 Keil、STM32CubeMX 和 CCS 工程引用关系不被破坏，仓库保留了原有目录、工程配置、部分构建产物和历史版本。

## 项目状态

| 项目 | 状态 |
| --- | --- |
| 比赛成绩 | 2025 年全国大学生电子设计竞赛 E 题省级一等奖 |
| MSPM0 工程 | 已由压缩包恢复为普通目录，共保留 5 个历史版本 |
| STM32 工程 | 已展开，保留原 Keil、CubeMX、HAL 和 CMSIS 结构 |
| 视觉部分 | 未收录 |
| 硬件 PCB | 未收录 |
| 当前验证情况 | 已检查工程文件完整性，尚未在整理后的环境中重新全量编译、烧录和整机验证 |

芯片、软件版本和工程兼容性说明请参阅 [芯片与开发环境说明.docx](./芯片与开发环境说明.docx)。

## 系统组成

| 模块 | 硬件/方案 | 主要作用 | 仓库收录情况 |
| --- | --- | --- | --- |
| 视觉识别 | K230 | 识别靶心并输出目标信息 | 未收录视觉代码 |
| 小车控制 | MSPM0G3507 | 小车循迹与底盘控制 | 已收录 |
| 云台控制 | STM32H723ZGT6 | 步进云台、搜索与瞄准控制 | 已收录 |
| 云台执行机构 | 步进电机 + 轮趣双路步进电驱 | 驱动步进云台 | 相关控制代码已收录 |
| 行走机构 | 减速电机 + DRV8701 | 驱动车体运动 | 相关控制代码已收录 |

## 工程结构

```text
UNEDC_25_E_Code/
├─ M0MSP3507/
│  ├─ test_1.4/                # MSPM0G3507 历史版本
│  ├─ test_1.5/                # MSPM0G3507 历史版本
│  ├─ test_1.6/                # MSPM0G3507 历史版本
│  ├─ test_1.7/                # MSPM0G3507 历史版本
│  └─ test_1.7.1/             # MSPM0G3507 的后期版本，建议从这里开始阅读
├─ STM32H723ZGT6/
│  ├─ H723_Car/                # H723 小车控制工程
│  ├─ H723_Car_1.9/            # H723_Car 1.9 历史版本
│  ├─ H723_Car_2.0/            # H723_Car 2.0 历史版本
│  ├─ H723_Motor/              # H723 电机控制与调试工程
│  ├─ H723_Searching/          # 搜索、循迹相关工程
│  ├─ H723_Stepper/            # H723 步进云台工程
│  ├─ H723_Stepper_Archive/   # 从冲突压缩包恢复的另一份历史快照
│  ├─ Stepper_Control/         # 步进电机控制工程
│  ├─ Stepper_Motor/          # 实际目标芯片为 STM32F103C8
│  └─ STM32H723_Tripod/       # H723 三脚架/机构控制工程
├─ 芯片与开发环境说明.docx
├─ LICENSE
└─ README.md
```

### 子工程说明

| 子目录 | 说明 |
| --- | --- |
| `M0MSP3507/test_1.4` ～ `test_1.7.1` | MSPM0G3507 小车控制在比赛期间形成的连续历史版本，并非五个独立功能模块 |
| `H723_Car` | H723 小车控制基线工程 |
| `H723_Car_1.9` | H723_Car 1.9 历史版本 |
| `H723_Car_2.0` | H723_Car 2.0 历史版本；实际工程根目录位于其内部的 `H723_Car/` |
| `H723_Motor` | H723 电机控制与调试工程 |
| `H723_Searching` | 搜索、循迹与目标扫描相关工程 |
| `H723_Stepper` | H723 步进云台控制工程的当前展开版本 |
| `H723_Stepper_Archive` | 与 `H723_Stepper` 存在源码及 Keil 配置差异的历史快照 |
| `Stepper_Control` | 步进电机控制与执行机构调试工程 |
| `Stepper_Motor` | STM32F103C8 辅助步进电机工程，不是 H723 工程 |
| `STM32H723_Tripod` | H723 三脚架/机构控制相关工程 |

### MSPM0 / CCS 通用工程结构

`M0MSP3507` 下的五个版本都采用同一套 CCS 工程模板。以下以 `test_1.7.1` 为例：

```text
test_1.7.1/
├─ .settings/                 # CCS/Eclipse 工程设置
├─ Debug/                     # CCS 调试构建产物，已由 .gitignore 排除
├─ targetConfigs/             # 目标器件与调试连接配置
├─ .ccsproject                # CCS 工程描述
├─ .cproject                  # CDT 构建配置、编译器和 SDK 版本
├─ .project                   # Eclipse 工程入口与工程名称
├─ .clangd                    # Clangd 代码索引配置
├─ empty.syscfg               # MSPM0G3507 引脚和外设的 SysConfig 配置
├─ empty.c                    # 主程序及初始化入口
├─ Motor.c / Motor.h          # 电机控制
├─ PID.c / PID.h              # PID 控制
├─ Key.c / Key.h              # 按键输入
├─ OLED.c / OLED.h            # OLED 驱动
├─ OLED_Data.c / OLED_Data.h  # OLED 字模/显示数据
├─ gray_detection.c / .h     # 灰度传感与循迹相关逻辑
└─ README.*                   # TI/工程说明文件
```

不同版本的目录结构基本一致，主要差异集中在业务源码、控制参数和调试过程。学习时不需要分别理解五套工程框架，只需以 `test_1.7.1` 熟悉一次 CCS 模板，再使用 Git 对比其他版本。

### STM32 / CubeMX / Keil 通用工程结构

STM32 子工程采用相近的 STM32CubeMX + HAL + Keil MDK 模板。不同工程可能缺少某些可选目录，以下以 `H723_Car` 为代表：

```text
H723_Car/
├─ Code/                      # 菜单、协议解析、PID 等应用层代码
├─ Core/
│  ├─ Inc/                    # CubeMX 生成的核心头文件
│  └─ Src/                    # main、外设初始化、中断与 HAL MSP 代码
├─ Drivers/
│  ├─ CMSIS/                  # Arm 内核与芯片设备层支持
│  └─ STM32H7xx_HAL_Driver/   # STM32H7 HAL 驱动
├─ Hardware/                  # OLED、MPU6050、电机等板级驱动
├─ System/                    # 延时等系统级辅助模块
├─ MDK-ARM/
│  ├─ DebugConfig/            # Keil 调试器配置
│  ├─ RTE/                    # Keil Runtime Environment 配置
│  ├─ *.uvprojx               # Keil 工程入口
│  ├─ *.uvoptx                # Keil 工程选项
│  └─ <Target>/               # 对象文件、固件和映射文件等构建产物
├─ .mxproject                 # STM32CubeMX 工程元数据
└─ *.ioc                      # 引脚、时钟和外设配置
```

`Stepper_Motor` 使用的仍是这套模板，只是目标芯片和驱动目录改为 STM32F103C8 与 `STM32F1xx_HAL_Driver`。因此 STM32 部分只需掌握一次模板结构，再结合各子工程的 `Code`、`Hardware` 和 `Core/Src` 阅读具体功能。

### 历史版本说明

- `M0MSP3507/test_1.4` 至 `test_1.7.1` 是比赛期间逐步形成的版本，并非五个独立功能模块。
- `H723_Stepper` 与 `H723_Stepper_Archive` 的源码和 Keil 工程配置存在差异，因此两份版本均被保留，便于对照学习。
- `Stepper_Motor` 虽位于 `STM32H723ZGT6` 总目录下，但其 `.ioc` 和 `.uvprojx` 指向 **STM32F103C8**，打开时不要选择 H723 器件。

## 开发环境

以下版本来自仓库中的 `.cproject`、`.syscfg`、`.ioc`、`.uvprojx` 和构建日志，是复现旧工程时的优先参考，并不代表官网当前最新版。

### MSPM0G3507

| 组件 | 工程记录版本 |
| --- | --- |
| 开发环境 | Code Composer Studio（CCS） |
| MSPM0 SDK | 2.5.1.00 |
| SysConfig | 1.24.0 |
| 编译器 | TI Arm Clang 4.0.3 LTS |
| CCS 构建系统 | CCS MBS 70.0.0 |
| 目标器件 | MSPM0G3507，LQFP-64（PM） |

### STM32H723ZGT6

| 组件 | 工程记录版本 |
| --- | --- |
| STM32CubeMX | 6.8.1 |
| STM32CubeH7 | FW_H7 V1.11.2 |
| Keil µVision / MDK-ARM | 5.42.0.0 |
| Arm Compiler | 6.23（ARMCLANG） |
| STM32H7 Device Family Pack | Keil.STM32H7xx_DFP 4.1.0 |
| CMSIS Pack | ARM.CMSIS 6.1.0 |

### STM32F103C8 辅助工程

| 组件 | 工程记录版本 |
| --- | --- |
| STM32CubeMX | 6.8.1 |
| STM32CubeF1 | FW_F1 V1.8.6 |
| Arm Compiler | 6.23（ARMCLANG） |
| STM32F1 Device Family Pack | Keil.STM32F1xx_DFP 2.2.0 |

## 快速开始

### 1. 克隆仓库

```bash
git clone https://github.com/IcksLin/UNEDC_25_E_Code.git
cd UNEDC_25_E_Code
```

建议将仓库放在较短的英文路径下，降低旧版嵌入式工具对长路径和非 ASCII 路径的兼容风险。

### 2. 打开 MSPM0 工程

1. 安装 Code Composer Studio、MSPM0 SDK 2.5.1 和 SysConfig 1.24.0。
2. 在 CCS 中选择 **Import Existing CCS Projects**。
3. 选择一个版本目录，例如 `M0MSP3507/test_1.7.1`。
4. 确认目标器件为 `MSPM0G3507`，编译器为 `TI Arm Clang 4.0.3 LTS`。
5. 双击 `empty.syscfg` 可查看引脚与外设配置。

### 3. 打开 STM32 工程

1. 安装 Keil MDK、工程记录的 Device Family Pack 和 Arm Compiler 6.23。
2. 进入目标工程的 `MDK-ARM` 目录。
3. 使用 Keil µVision 打开对应的 `.uvprojx` 文件。
4. 需要修改引脚、时钟或外设时，再使用 STM32CubeMX 6.8.1 打开 `.ioc` 文件。

例如，小车 2.0 版本的 Keil 工程入口为：

```text
STM32H723ZGT6/H723_Car_2.0/H723_Car/MDK-ARM/H723_Car.uvprojx
```

> [!WARNING]
> 不建议首次打开时直接接受 IDE 的工程升级、器件包迁移或 CubeMX 重新生成。请先复制工程或新建 Git 分支，再检查升级前后的差异。CubeMX 重新生成代码前，还应确认自定义逻辑位于 `USER CODE` 区域。

## 竞赛历程

- **第一天：** 完成整体结构搭建；同期队友完成视觉部分代码。
- **第二天：** 实现小车循迹。
- **第三、四天：** 持续调试循迹、搜索与瞄准流程，完成整机联调。

## 经验复盘

### 1. 机械结构必须尽早闭环验证

前期结构设计阶段已经意识到云台连续旋转可能造成线束缠绕甚至绞死，但没有及时落实解决方案。这是本次作品未能继续取得更好成绩的重要原因之一。

金属牛眼轮对地面材质、灰尘和装配状态较为敏感，轻微变化就可能引起车体震动。整车结构、轮组选择、重心和线束路径都应尽早在真实场地上验证，而不能只依赖静态设计。

### 2. 驱动方案需要与负载匹配

轮趣双路步进电驱在小电流条件下难以提供足够转矩；同时，硬件没有编码器检测回路，自行编写的开环步进控制也存在丢步后无法感知的问题。综合来看，这套驱动方案并不适合当时的任务场景。

步进电机的脉冲开环控制避免了部分闭环 PID 中的震荡和积分延迟问题，但代价是对轨迹规划、脉冲频率和机械负载变化更敏感。若改用舵机配合 PID，应优先选择精度与响应更好的数字舵机，模拟舵机可能难以满足瞄准精度要求。

### 3. 控制算法仍有优化空间

如果重新设计，可以考虑：

- 对激光点或靶心位置加入 α-β 滤波、状态预测或其他轻量预测算法；
- 在云台闭环中加入前馈项，减小目标快速变化时的滞后；
- 在直角转弯等姿态快速变化阶段，使用陀螺仪为云台提供补偿；
- 对搜索、锁定、丢失目标和重新捕获建立更清晰的状态机。

云台补偿的思路与 RoboMaster 中常见的“小陀螺”姿态补偿存在一定相似性，但本项目没有进一步验证这一方案。

### 4. 传感器必须具备健康检查

MPU6050 在比赛现场出现异常，但程序缺少可靠的初始化校验和运行状态检查，导致问题没有被及时发现。传感器初始化不应只执行写寄存器操作，还应读取器件 ID、关键配置和数据有效性，并在异常时提供可观察的错误状态。

### 5. PCB、供电与重心同样影响控制效果

当时的 H7 扩展板按照嘉立创 10 cm × 10 cm 规格拆分为三块板，结构搭建成本较高，最终车体体积偏大、重心偏高。整车采用双电池，一方面用于压低重心，另一方面用于隔离步进电机持续高功耗造成的电池压降，减少其对小车循迹控制的影响。

硬件设计应尽量小巧、稳定并保留调整空间。电源分区、地线规划、驱动峰值电流、传感器安装位置和机械结构，都可能直接影响最终控制效果。

## 已知限制

- 仓库不包含 K230 视觉识别代码。
- 仓库不包含 PCB、原理图和机械结构设计文件。
- 多个工程包含历史构建产物、IDE 用户配置和调试日志。
- 工程尚未在当前整理环境中重新完成全量编译、烧录和整机测试。
- 不同版本的 Keil、Arm Compiler、Device Family Pack、STM32CubeMX 或 MSPM0 SDK 可能触发迁移并改变工程文件。

## 学习建议

1. 先阅读 [芯片与开发环境说明.docx](./芯片与开发环境说明.docx)，准备接近原工程的工具版本。
2. MSPM0 部分建议从 `test_1.7.1` 开始，再向前比较历史版本。
3. STM32 部分优先阅读 `Code`、`Hardware`、`Core/Src` 和对应的 `.uvprojx`、`.ioc` 文件。
4. 使用 Git 对比 `H723_Stepper` 与 `H723_Stepper_Archive`，观察比赛期间源码和工程配置的变化。
5. 修改或迁移前先建立独立分支，避免直接覆盖历史工程。

## License

本项目采用 [MIT License](./LICENSE)。

如果本仓库能为后续参赛同学提供一些思路或避坑经验，那么这次整理就有意义。
