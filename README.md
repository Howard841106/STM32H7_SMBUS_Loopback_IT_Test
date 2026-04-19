# STM32H7 SMBus Loopback Test

## 📌 Overview

This project demonstrates a **SMBus loopback communication** on a single STM32H7 board using two SMBus peripherals.

It verifies data transmission by sending a test string from master to slave and comparing the received data.

---

## 🧰 Hardware

* STM32H7 series (e.g., NUCLEO-H723ZG)
* SMBus1 ↔ SMBus2 (Pin-to-Pin connection)
* Pull-up resistors (recommended: 4.7kΩ)

---

## ⚙️ Configuration

### SMBus Settings

* Mode: SMBus
* Addressing Mode: 7-bit
* Slave Address: `0x02`

```c
#define SMBUS_SLAVE_ADDR   (2)
```

---

## 🚀 Function Description

### Loopback Flow

1. Master sends data to Slave
2. Slave receives data (Interrupt mode)
3. Store received data in `rxData`
4. Compare with `txData`
5. Output result via UART

---

## 📤 UART Debug Output

Using USART3 (ST-Link VCP) for printf output:

```
============== SMBus Loopback Test Start ==============
TX ASCII : HELLO_SMBUS.
RX ASCII : HELLO_SMBUS.
TX HEX   : 48 45 4C 4C 4F 5F 53 4D 42 55 53 00
RX HEX   : 48 45 4C 4C 4F 5F 53 4D 42 55 53 00
```

---

## 🧪 Test Result

* PASS: txData == rxData
* FAIL: Data mismatch

---

## 🔧 Key Function

```c
static TestResult_t Check_I2C_Data(void)
{
  if (memcmp(txData, rxData, sizeof(txData)) == 0)
  {
    return TEST_PASS;
  }

  printf("Rx Data = %s\r\n", rxData);
  return TEST_MISMATCH;
}
```

---

## ⚠️ Notes

* Ensure both SMBus peripherals use the same slave address
* Pull-up resistors are required for stable communication
* Interrupt mode must be properly enabled
* USART3 must be initialized before using printf

---

## 🐞 Common Issues

### huart3 undeclared

* Make sure `MX_USART3_UART_Init()` is enabled
* Ensure `huart3` is declared

### Data mismatch

* Check SMBus address
* Verify timing configuration
* Confirm wiring connection

---

## 📁 Project Structure

```
/Src
  main.c
  smbus.c
  usart.c

/Inc
  main.h
```

---

## 📈 Future Improvements

* Add CRC verification
* Support command protocol
* Add retry mechanism
* Compare interrupt vs polling mode

---

## 👨‍💻 Author

Howard Chen
Firmware R&D Engineer
