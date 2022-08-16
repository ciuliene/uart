# Uart

- [Uart](#uart)
  - [1. Dynamic-Link Library](#1-dynamic-link-library)
  - [2. Functions](#2-functions)
    - [2.1. CreateComm](#21-createcomm)
    - [2.2. SendCommand](#22-sendcommand)
    - [2.3. ReadCommand](#23-readcommand)
    - [2.4. SRCommand](#24-srcommand)
    - [2.5. ModbusCalcCRC](#25-modbuscalccrc)
    - [2.6. CloseComm](#26-closecomm)
    - [2.7. GetAvailableCom](#27-getavailablecom)

## 1. Dynamic-Link Library

This repository contains the code to generate a dll file used to communicate with remote devices through UART.

NOTE: currently this code can run only on Windows OS.

## 2. Functions

This is the list of functions provided by the DLL:

- CreateComm
- SendCommand
- ReadCommand
- SRCommand
- ModbusCalcCRC
- CloseComm
- GetAvailableCom

Functions and parameters are described below.

### 2.1. CreateComm

This function opens a serial port and returns a handler for the port.
If something goes wrong, the function returns an error (for example whether the port is already open by another application).

```c
char *nCom // COM1, COM2, etc.
int baud // Baud rate (list of available baud rates below)
char *conf // Configuration for the communication (description below)
```

List of Available baud rates:

- 110
- 300
- 600
- 1200
- 2400
- 4800
- 9600
- 14400
- 19200
- 38400
- 56000
- 57600
- 115200
- 128000
- 256000
- 500000
- 1000000

Configuration for the communication:

- N_BITS (5,6,7,8)
- PARITY (n, e, o)
- STOP_BITS (1, 2)

Example: `8n1`

### 2.2. SendCommand

This function sends a message to the connected device and returns the number of bytes sent.

```c
HWND hPort // Handler of the serial port (returned by CreateComm function)
unsigned char *send // The message to send
unsigned int lenSend // The length of the message to send
```

### 2.3. ReadCommand

This function reads the buffer and returns the number of byte received.

```c
HWND hPort // Handler of the serial port (returned by CreateComm function)
unsigned char *recv // The pointer of the buffer where to save the response
unsigned int lenFrame // The maximum number of bytes to receive
```

### 2.4. SRCommand

This function combines the functions described above (SendCommand and ReceiveCommand).

```c
HWND hPort // Handler of the serial port (returned by CreateComm function)
unsigned char *send // The message to send
unsigned int lenS  // The length of the message to send
unsigned char *recv // The pointer of the buffer where to save the response
unsigned int lenR // The maximum number of bytes to receive
unsigned int delay // The delay in milliseconds between sending and receiving
```

### 2.5. ModbusCalcCRC

This function calculate the CRC of a buffer using CRC-16/MODBUS algorithm. It returns a 16 bit value, two bytes that must be swapped.

```c
unsigned char *Frame // The pointer of the buffer
int lenFrame // The length of the message
```

### 2.6. CloseComm

This function close the connection with the serial port.

```c
HWND hPort // Handler of the serial port (returned by CreateComm function)
```

### 2.7. GetAvailableCom

This function gets the list of available serial ports, checking the ports between COM1 and COM100. It returns the number of available ports.

```c
char *ncom // The buffer where to save the list of the available serial ports.
```
