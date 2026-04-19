# 🔌 STM32 ST-Link VCP Usage Guide

---

## 📖 1. What is STLK_VCP_TX?

**STLK_VCP_TX** is the TX pin of the Virtual COM Port (VCP) provided by the on-board ST-Link on STM32 Nucleo boards.

👉 It converts MCU UART into USB communication, allowing the PC to access it as a COM port.

---

## 📌 2. Signal Mapping

| Name | Direction | Description |
|------|----------|------------|
| STLK_VCP_TX | ST-Link → MCU | Connect to MCU RX |
| STLK_VCP_RX | MCU → ST-Link | Connect to MCU TX |

---

## 🔧 3. Hardware Connection (NUCLEO-H723ZG)

| MCU Pin | Function | Mapping |
|---------|----------|---------|
| PD8 | USART3_TX | → STLK_VCP_RX |
| PD9 | USART3_RX | ← STLK_VCP_TX |

### 📌 Wiring Rule

```
MCU_TX → ST-Link RX  
MCU_RX ← ST-Link TX
```

👉 Always connect TX to RX

---

## ⚙️ 4. CubeMX Configuration

- Mode: Asynchronous  
- Baudrate: 115200  
- TX: PD8  
- RX: PD9  

---

## 💻 5. printf Redirection

```c
#include <stdio.h>

int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
```

### Usage

```c
printf("Hello STM32\r\n");
```

---

## 🖥️ 6. Mac Usage

### List devices

```bash
ls /dev/cu.*
```

Example:

```
/dev/cu.usbmodemXXXXX
```

### Open terminal

```bash
screen /dev/cu.usbmodemXXXXX 115200
```

### Exit screen

```
Ctrl + A → K → Y
```

---

## 🖥️ 7. Windows Usage

### Check COM Port

Device Manager will show:

```
STMicroelectronics STLink Virtual COM Port (COMx)
```

### Recommended Tools

- PuTTY  
- Tera Term  

Settings:

- Baudrate: 115200  
- Data: 8  
- Parity: None  
- Stop: 1  

---

## 🔄 8. RX Support

✅ Full duplex supported

```c
uint8_t rx;
HAL_UART_Receive(&huart3, &rx, 1, HAL_MAX_DELAY);
```

---

## ⚠️ 9. Common Issues

### ❌ No output

- TX/RX reversed  
- Baudrate mismatch  
- printf not redirected  

### ❌ COM Port not found

- USB not connected  
- Driver issue  
- Permission issue  

### ❌ printf blocking

```c
HAL_UART_Transmit(... HAL_MAX_DELAY)
```

👉 Blocking behavior (expected)

---

## 💡 10. Practical Usage

### Debug Log

```c
printf("[INFO] Init Done\r\n");
```

### Error Log

```c
printf("[ERROR] I2C Fail\r\n");
```

### CLI Control (Advanced)

```
> start
> stop
> set current 10
```

---

## 🧠 11. Core Concept

```
UART ⇄ ST-Link ⇄ USB ⇄ PC Terminal
```

👉 It is still UART at its core

---

## 🧑‍💻 12. Quick Start for Users (Mac & Windows)

### 🔹 Mac Users

1. Plug in the Nucleo board  
2. Open Terminal  
3. Run:

```bash
ls /dev/cu.*
```

4. Find device (e.g. `/dev/cu.usbmodem11403`)  
5. Connect:

```bash
screen /dev/cu.usbmodem11403 115200
```

👉 You should see printf output  

---

### 🔹 Windows Users

1. Plug in the Nucleo board  
2. Open Device Manager  
3. Find:

```
STLink Virtual COM Port (COMx)
```

4. Open PuTTY / Tera Term  
5. Configure:

- COM Port: COMx  
- Baudrate: 115200  

👉 You should see output  

---

### 🔸 Common Beginner Mistakes

- Terminal not opened  
- Wrong COM port  
- Baudrate mismatch  
- Missing printf redirect  

---

## 🚀 13. Advanced Applications

- Debug Console  
- RS232 / I2C control tools  
- Firmware parameter tuning  
- Logging system  

---
