# pic18f_spi_oled_driver

## Introduction

This is a driver for the **PIC18F4520** that controls an SPI display to render characters.

Characters can be sent to the PIC18F4520 through UART, and the driver will display the received input on the screen.

This project is designed to work with the **PICOS** project. Some parts of the code are simplified based on PICOS-specific assumptions, so correct behavior is not guaranteed when used in other environments.

## Features

* Real-time processing of character input to ensure no characters are lost under the current baud rate and clock configuration
* Display printable characters received from UART
* Automatically move to the next line when a line is full
* Automatically scroll down when the screen is full
* **INT1** scrolls the screen down
* **INT2** scrolls the screen up
* Typing after scrolling up automatically returns the display to the cursor position
* Support for **backspace** character
* Support for **newline** character
* **0x3** clears the screen
