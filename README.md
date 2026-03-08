# dsp.ti
a simple project processing with dsp
针对 TI TMS320C6678 的例程和调试说明书。

多核开发的痛点往往不在于单核跑代码，而在于核间通信（IPC）、资源分配以及多核同步调试。

这份说明书模板为你梳理了最核心的框架，你可以直接复制到你的 README.md 中，并根据你实际写的代码进行微调。

🚀 TI TMS320C6678 多核 DSP 开发与调试指南
1. 项目简介 (Introduction)
本项目包含了基于 TI TMS320C6678 多核 DSP 的基础驱动例程、核间通信（IPC）机制演示，以及多核协同处理的测试代码。旨在帮助开发者快速熟悉 C6678 的硬件架构，掌握 Code Composer Studio (CCS) 下的多核调试技巧。

2. 开发环境准备 (Environment Setup)
在运行本项目例程之前，请确保你的开发环境已正确配置：

集成开发环境 (IDE)： Code Composer Studio (CCS) v9.0 或以上版本。

软件开发套件 (SDK)： TI MCSDK (Multicore Software Development Kit) 或更现代的 Processor SDK RTOS。

实时操作系统： TI-RTOS (SYS/BIOS)。

硬件仿真器： XDS100v2 / XDS200 / XDS560v2。

3. 核心例程说明 (Core Routines)
本项目包含以下核心模块的例程，按照从单核到多核的进阶顺序排列：

🌱 基础单核例程 (Basic Single-Core)
01_GPIO_LED：最基础的流水灯实验，验证 DSP 核心的启动与 GPIO 基本引脚控制。

02_Timer_Interrupt：定时器与中断控制器（INTC）配置，验证中断响应机制。

⚡ 高速外设例程 (High-Speed Peripherals)
03_EDMA3_Transfer：使用 EDMA3 进行内存到内存（DDR3 与 L2 Cache 之间）的高效数据搬移，不占用 CPU 资源。

04_SRIO_Loopback：SRIO（Serial RapidIO）接口的内部回环测试，用于板间或芯片间的高速通信。

🧠 多核协同例程 (Multi-Core Collaboration)
05_IPC_MessageQ：基于 SYS/BIOS 的 MessageQ 模块实现 Core 0 与 Core 1-7 之间的消息传递与数据指针共享。

06_OpenMP_Test：基于 OpenMP 的多核并行计算演示，快速将单核的 for 循环任务分配到 8 个核心上同时执行。

4. 多核调试核心方法 (Debugging Methods)
调试多核 DSP 与单核单片机有本质区别。请务必掌握以下 CCS 调试技巧：

🔌 4.1 目标配置文件 (.ccxml) 创建
在 CCS 中新建 Target Configuration File。

选择你的仿真器类型（如 Texas Instruments XDS200 USB Debug Probe）。

目标芯片选择 TMS320C6678，保存并点击 Test Connection 确保底层 JTAG 链路畅通。

🔗 4.2 多核同步启动与运行 (Sync Run/Halt)
Group Cores (核心编组)： 在 Debug 视图中，选中 Core 0 到 Core 7，右键选择 Group core(s)。这样点击一次“运行 (Resume)”或“暂停 (Suspend)”，8 个核心会同步动作，防止出现某些核心跑飞、某些核心未启动的尴尬局面。

独立加载程序： Core 0 通常作为主控核心（Master），负责系统初始化和代码分发；其他核心作为从核（Slave）。你需要分别为不同的核心 Load Program (.out 文件)。

🛠️ 4.3 内存与寄存器查看
Memory Browser： C6678 存在私有内存（L1/L2 Cache）和共享内存（MSMC RAM, DDR3）。在 Memory Browser 中输入地址时，注意区分全局地址和本地地址。

Registers View： 重点关注 Core Registers (如 PC, SP) 和外设配置寄存器。在调试 IPC 时，需实时查看 IPC Generation Registers (IPCGR) 的状态。

📊 4.4 SYS/BIOS 系统级调试 (ROV)
如果例程基于 TI-RTOS 运行，传统的断点调试可能会打乱系统的实时性。

打开 Tools -> RTOS Object View (ROV)。

在这里可以在不暂停 DSP 运行的情况下，实时查看任务（Task）的堆栈使用率、CPU 负载、MessageQ 的消息队列深度，是排查多核死锁的神器。
