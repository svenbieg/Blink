//==============
// GpioHelper.h
//==============

#pragma once


//===============
// Documentation
//===============

//			|	Alt0			Alt1		Alt2			Alt3			Alt4			Alt5			Alt6			Alt7			Alt8
//-----------------------------------------------------------------------------------------------------------------------------------------------------
//	GPIO0	|	SPI0_SIO[3]		DPI_PCLK	UART1_TX		I2C0_SDA		SYS_RIO[0]		PROC_RIO[0]		PIO[0]			SPI2_CSn[0]		
//	GPIO1	|	SPI0_SIO[2]		DPI_DE		UART1_RX		I2C0_SCL		SYS_RIO[1]		PROC_RIO[1]		PIO[1]			SPI2_SIO[1]		
//	GPIO2	|	SPI0_CSn[3]		DPI_VSYNC	UART1_CTS		I2C1_SDA		UART0_IR_RX		SYS_RIO[2]		PROC_RIO[2]		PIO[2]			SPI2_SIO[0]
//	GPIO3	|	SPI0_CSn[2]		DPI_HSYNC	UART1_RTS		I2C1_SCL		UART0_IR_TX		SYS_RIO[3]		PROC_RIO[3]		PIO[3]			SPI2_SCLK
//	GPIO4	|	GPCLK[0]		DPI_D[0]	UART2_TX		I2C2_SDA		UART0_RI		SYS_RIO[4]		PROC_RIO[4]		PIO[4]			SPI3_CSn[0]
//	GPIO5	|	GPCLK[1]		DPI_D[1]	UART2_RX		I2C2_SCL		UART0_DTR		SYS_RIO[5]		PROC_RIO[5]		PIO[5]			SPI3_SIO[1]
//	GPIO6	|	GPCLK[2]		DPI_D[2]	UART2_CTS		I2C3_SDA		UART0_DCD		SYS_RIO[6]		PROC_RIO[6]		PIO[6]			SPI3_SIO[0]
//	GPIO7	|	SPI0_CSn[1]		DPI_D[3]	UART2_RTS		I2C3_SCL		UART0_DSR		SYS_RIO[7]		PROC_RIO[7]		PIO[7]			SPI3_SCLK
//	GPIO8	|	SPI0_CSn[0]		DPI_D[4]	UART3_TX		I2C0_SDA		SYS_RIO[8]		PROC_RIO[8]		PIO[8]			SPI4_CSn[0]		
//	GPIO9	|	SPI0_SIO[1]		DPI_D[5]	UART3_RX		I2C0_SCL		SYS_RIO[9]		PROC_RIO[9]		PIO[9]			SPI4_SIO[0]		
//	GPIO10	|	SPI0_SIO[0]		DPI_D[6]	UART3_CTS		I2C1_SDA		SYS_RIO[10]		PROC_RIO[10]	PIO[10]			SPI4_SIO[1]		
//	GPIO11	|	SPI0_SCLK		DPI_D[7]	UART3_RTS		I2C1_SCL		SYS_RIO[11]		PROC_RIO[11]	PIO[11]			SPI4_SCLK		
//	GPIO12	|	PWM0[0]			DPI_D[8]	UART4_TX		I2C2_SDA		AUDIO_OUT_L		SYS_RIO[12]		PROC_RIO[12]	PIO[12]			SPI5_CSn[0]
//	GPIO13	|	PWM0[1]			DPI_D[9]	UART4_RX		I2C2_SCL		AUDIO_OUT_R		SYS_RIO[13]		PROC_RIO[13]	PIO[13]			SPI5_SIO[1]
//	GPIO14	|	PWM0[2]			DPI_D[10]	UART4_CTS		I2C3_SDA		UART0_TX		SYS_RIO[14]		PROC_RIO[14]	PIO[14]			SPI5_SIO[0]
//	GPIO15	|	PWM0[3]			DPI_D[11]	UART4_RTS		I2C3_SCL		UART0_RX		SYS_RIO[15]		PROC_RIO[15]	PIO[15]			SPI5_SCLK
//	GPIO16	|	SPI1_CSn[2]		DPI_D[12]	MIPI0_DSI_TE	UART0_CTS		SYS_RIO[16]		PROC_RIO[16]	PIO[16]
//	GPIO17	|	SPI1_CSn[1]		DPI_D[13]	MIPI1_DSI_TE	UART0_RTS		SYS_RIO[17]		PROC_RIO[17]	PIO[17]
//	GPIO18	|	SPI1_CSn[0]		DPI_D[14]	I2S0_SCLK		PWM0[2]			I2S1_SCLK		SYS_RIO[18]		PROC_RIO[18]	PIO[18]			GPCLK[1]
//	GPIO19	|	SPI1_SIO[1]		DPI_D[15]	I2S0_WS			PWM0[3]			I2S1_WS			SYS_RIO[19]		PROC_RIO[19]	PIO[19]	
//	GPIO20	|	SPI1_SIO[0]		DPI_D[16]	I2S0_SDI[0]		GPCLK[0]		I2S1_SDI[0]		SYS_RIO[20]		PROC_RIO[20]	PIO[20]	
//	GPIO21	|	SPI1_SCLK		DPI_D[17]	I2S0_SDO[0]		GPCLK[1]		I2S1_SDO[0]		SYS_RIO[21]		PROC_RIO[21]	PIO[21]	
//	GPIO22	|	SDIO0_CLK		DPI_D[18]	I2S0_SDI[1]		I2C3_SDA		I2S1_SDI[1]		SYS_RIO[22]		PROC_RIO[22]	PIO[22]	
//	GPIO23	|	SDIO0_CMD		DPI_D[19]	I2S0_SDO[1]		I2C3_SCL		I2S1_SDO[1]		SYS_RIO[23]		PROC_RIO[23]	PIO[23]	
//	GPIO24	|	SDIO0_DAT[0]	DPI_D[20]	I2S0_SDI[2]		I2S1_SDI[2]		SYS_RIO[24]		PROC_RIO[24]	PIO[24]			SPI2_CSn[1]	
//	GPIO25	|	SDIO0_DAT[1]	DPI_D[21]	I2S0_SDO[2]		AUDIO_IN_CLK	I2S1_SDO[2]		SYS_RIO[25]		PROC_RIO[25]	PIO[25]			SPI3_CSn[1]
//	GPIO26	|	SDIO0_DAT[2]	DPI_D[22]	I2S0_SDI[3]		AUDIO_IN_DAT0	I2S1_SDI[3]		SYS_RIO[26]		PROC_RIO[26]	PIO[26]			SPI5_CSn[1]
//	GPIO27	|	SDIO0_DAT[3]	DPI_D[23]	I2S0_SDO[3]		AUDIO_IN_DAT1	I2S1_SDO[3]		SYS_RIO[27]		PROC_RIO[27]	PIO[27]			SPI1_CSn[1]

//			|	Func1		Func2		Func3				Func4			Func5	Func6	Func7	Func8
//--------------------------------------------------------------------------------------------------------
// GPIO30	|												WIFI_SDIO_CLK
// GPIO31	|												WIFI_SDIO_CMD
// GPIO32	|												WIFI_SDIO_D0
// GPIO33	|							WIFI_SDIO_D1
// GPIO34	|												WIFI_SDIO_D2
// GPIO35	|							WIFI_SDIO_D3


//=======
// Using
//=======

#include "Devices/System/Interrupts.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Gpio {


//======
// Pins
//======

enum class GpioArmPin
{
Gpio28=28,
Gpio30=30,
Gpio31=31,
Gpio32=32,
Gpio33=33,
Gpio34=34,
Gpio35=35,
Gpio41=41,
WifiOn=28,
WifiSdioClk=30,
WifiSdioCmd=31,
WifiSdioD0=32,
WifiSdioD1=33,
WifiSdioD2=34,
WifiSdioD3=35,
ActivityLed=41,
CpuFan=45
};

enum class GpioRp1Pin
{
Gpio0, Gpio1, Gpio2, Gpio3,
Gpio4, Gpio5, Gpio6, Gpio7,
Gpio8, Gpio9, Gpio10, Gpio11,
Gpio12, Gpio13, Gpio14, Gpio15,
Gpio16, Gpio17, Gpio18, Gpio19,
Gpio20, Gpio21, Gpio22, Gpio23,
Gpio24, Gpio25, Gpio26, Gpio27
};


//==========
// Pin-Mode
//==========

enum class GpioPinMode
{
Input,
Output
};

enum class GpioArmPinMode
{
Output,
Func1,
Func2,
Func3,
Func4,
Func5,
Func6,
Func7,
Func8
};

enum class GpioRp1PinMode
{
Input,
Output,
Alt0,
Alt1,
Alt2,
Alt3,
Alt4,
Alt5,
Alt6,
Alt7,
Alt8
};


//===========
// Pull-Mode
//===========

enum class GpioPullMode
{
None,
PullDown,
PullUp
};


//================
// Interrupt-Mode
//================

enum class GpioIrqMode
{
Edge,
FallingEdge,
RisingEdge
};


//======
// Gpio
//======

class GpioHelper
{
public:
	// Using
	using IRQ_HANDLER=Devices::System::IRQ_HANDLER;

	// Common
	static FLOAT AnalogRead(BYTE Pin);
	static BOOL DigitalRead(BYTE Pin);
	static BOOL DigitalRead(GpioArmPin Pin);
	static BOOL DigitalRead(GpioRp1Pin Pin);
	static VOID DigitalWrite(BYTE Pin, BOOL Value);
	static VOID DigitalWrite(GpioArmPin Pin, BOOL Value);
	static VOID DigitalWrite(GpioRp1Pin Pin, BOOL Value);
	static VOID SetInterruptHandler(BYTE Pin, IRQ_HANDLER Handler, VOID* Parameter=nullptr, GpioIrqMode Mode=GpioIrqMode::Edge);
	static VOID SetInterruptHandler(GpioRp1Pin Pin, IRQ_HANDLER Handler, VOID* Parameter=nullptr, GpioIrqMode Mode=GpioIrqMode::Edge);
	static VOID SetPinMode(BYTE Pin, GpioPinMode Mode, GpioPullMode PullMode=GpioPullMode::None);
	static VOID SetPinMode(GpioArmPin Pin, GpioArmPinMode Mode, GpioPullMode PullMode=GpioPullMode::None);
	static VOID SetPinMode(GpioRp1Pin Pin, GpioRp1PinMode Mode, GpioPullMode PullMode=GpioPullMode::None);
};

}}