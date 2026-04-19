# 🔌 STM32 ST-Link VCP 使用指南 (Bilingual Guide)

---

## 📖 1. 什麼是 STLK_VCP_TX？  
## 📖 1. What is STLK_VCP_TX?

**STLK_VCP_TX** 是 STM32 Nucleo 板上 ST-Link 提供的虛擬序列埠（VCP）TX 腳位。  
**STLK_VCP_TX** is the TX pin of the Virtual COM Port (VCP) provided by the on-board ST-Link.

👉 本質：將 MCU 的 UART 轉換為 USB 通訊  
👉 In essence: converts MCU UART into USB communication

---

## 📌 2. 對應關係  
## 📌 2. Signal Mapping

| 名稱 | 方向 | 說明 |
|------|------|------|
| STLK_VCP_TX | ST-Link → MCU | 接 MCU RX |
| STLK_VCP_RX | MCU → ST-Link | 接 MCU TX |

| Name | Direction | Description |
|------|----------|------------|
| STLK_VCP_TX | ST-Link → MCU | Connect to MCU RX |
| STLK_VCP_RX | MCU → ST-Link | Connect to MCU TX |

---

## 🔧 3. 硬體連接方式（NUCLEO-H723ZG）  
## 🔧 3. Hardware Connection

| MCU 腳位 | 功能 | 對應 |
|---------|------|------|
| PD8 | USART3_TX | → STLK_VCP_RX |
| PD9 | USART3_RX | ← STLK_VCP_TX |

### 📌 接線原則 / Wiring Rule

```
MCU_TX → ST-Link RX  
MCU_RX ← ST-Link TX
```

👉 TX 必須接 RX  
👉 TX must connect to RX

---

## ⚙️ 4. CubeMX 設定  
## ⚙️ 4. CubeMX Configuration

- Mode: Asynchronous  
- Baudrate: 115200  
- TX: PD8  
- RX: PD9  

---

## 💻 5. printf 重導  
## 💻 5. printf Redirection

```c
#include <stdio.h>

int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
```

### 使用方式 / Usage

```c
printf("Hello STM32\r\n");
```

---

## 🖥️ 6. Mac 使用方式  
## 🖥️ 6. Mac Usage

### 查詢裝置 / List devices

```bash
ls /dev/cu.*
```

範例 / Example:

```
/dev/cu.usbmodemXXXXX
```

### 開啟 Terminal / Open terminal

```bash
screen /dev/cu.usbmodemXXXXX 115200
```

### 離開 screen / Exit screen

```
Ctrl + A → K → Y
```

---

## 🖥️ 7. Windows 使用方式  
## 🖥️ 7. Windows Usage

### 查看 COM Port / Check COM port

裝置管理員會顯示：

```
STMicroelectronics STLink Virtual COM Port (COMx)
```

### 推薦工具 / Recommended Tools

- PuTTY  
- Tera Term  

設定 / Settings:

- Baudrate: 115200  
- Data: 8  
- Parity: None  
- Stop: 1  

---

## 🔄 8. 是否支援接收？  
## 🔄 8. RX Support

✅ 支援雙向通訊  
✅ Full duplex supported

```c
uint8_t rx;
HAL_UART_Receive(&huart3, &rx, 1, HAL_MAX_DELAY);
```

---

## ⚠️ 9. 常見問題  
## ⚠️ 9. Common Issues

### ❌ 沒有輸出 / No output

- TX/RX 接反  
- Baudrate 不一致  
- printf 未重導  

### ❌ 找不到 COM Port / COM Port not found

- USB 未連接  
- Driver 問題  
- 權限問題  

### ❌ printf 卡住 / printf blocking

```c
HAL_UART_Transmit(... HAL_MAX_DELAY)
```

👉 Blocking 行為（正常）  
👉 Blocking behavior (expected)

---

## 💡 10. 工程實務建議  
## 💡 10. Practical Usage

### Debug Log

```c
printf("[INFO] Init Done\r\n");
```

### Error Log

```c
printf("[ERROR] I2C Fail\r\n");
```

### CLI 控制（進階） / CLI Control (Advanced)

```
> start
> stop
> set current 10
```

---

## 🧠 11. 核心概念  
## 🧠 11. Core Concept

```
UART ⇄ ST-Link ⇄ USB ⇄ PC Terminal
```

👉 本質仍是 UART  
👉 It is still UART at its core

---

## 🧑‍💻 12. 使用者快速上手指南（Mac / Windows）  
## 🧑‍💻 12. Quick Start for Users (Mac & Windows)

### 🔹 Mac 使用者 / Mac Users

1. 插上 Nucleo 開發板  
2. 開 Terminal  
3. 輸入：

```bash
ls /dev/cu.*
```

4. 找到裝置（如 `/dev/cu.usbmodem11403`）  
5. 開啟連線：

```bash
screen /dev/cu.usbmodem11403 115200
```

👉 成功後即可看到 printf 輸出  

---

### 🔹 Windows 使用者 / Windows Users

1. 插上 Nucleo 開發板  
2. 開啟「裝置管理員」  
3. 找到：

```
STLink Virtual COM Port (COMx)
```

4. 開啟 PuTTY / Tera Term  
5. 設定：

- COM Port: COMx  
- Baudrate: 115200  

👉 連線後即可看到輸出  

---

### 🔸 常見新手錯誤 / Common Beginner Mistakes

- 沒開 Terminal / Serial Tool  
- COM Port 選錯  
- Baudrate 不一致  
- 沒有 printf redirect  

---

## 🚀 13. 延伸應用  
## 🚀 13. Advanced Applications

- Debug Console  
- RS232 / I2C 控制工具  
- 韌體參數調整  
- Log 系統  

---
