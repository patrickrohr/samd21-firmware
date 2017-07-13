/*
* UART.c
*
* Created: 6/12/2017 8:29:27 PM
*  Author: Patrick Rohr
*/ 

#include "UART.h"
#include "PortIO.h"
//#include "samd21g18a.h" // TODO: include sam21.h from now on....
//#include "PortMapping.h"




/*
what needs to happen in order:
1. clock init -> internal vs external clock

2. pin init: -> use PortIO
	TxD -> Output
	RxD -> Input
3. init
	-> start bit
	8 data bits
	big endian -> MSB first (lowest address)
	8N1 UART

use SERCOM0 for UART on pins 0/1

*/


// PRIVATE

// waits for CTRLB, ENABLE, or SWRST sync to happen
void _uart_sync(Sercom * self)
{
	while(self->USART.SYNCBUSY.reg);
}

void _uart_init(Sercom * self)
{
	self->USART.CTRLA.bit.MODE = 0x1;
	self->USART.CTRLA.bit.CMODE = 0x0;
	self->USART.CTRLA.bit.RXPO = 0x3;
	self->USART.CTRLA.bit.TXPO = 0x2;
	_uart_sync(self);
	self->USART.CTRLB.bit.CHSIZE = 0x0;
	_uart_sync(self);
	self->USART.CTRLA.bit.DORD = 0x0;
	self->USART.CTRLA.bit.FORM = 0x0;
	_uart_sync(self);
	self->USART.CTRLB.bit.SBMODE = 0x0;
	_uart_sync(self);

	// calculate baud rate. TODO, put in own function.
	//int baud_rate = 9600;
	//uint64_t baud_val = 65536.0 - 65536.0 * 3.0 * baud_rate / 48000000.0;
	
	self->USART.BAUD.bit.BAUD = 0xFFD8;//0xF5CC; // Baud Rate of 9600 with 3 samples per bit
	_uart_sync(self);
	self->USART.CTRLB.bit.RXEN = 1;
	self->USART.CTRLB.bit.TXEN = 1;
	_uart_sync(self);
	self->USART.INTENSET.bit.TXC = 1; // Transmitter interrupt enabled
	_uart_sync(self);
}

void _uart_reset(Sercom * self)
{
	// debugging...
	/*
	GENDIV.ID = // gen clock generator that will be selected
	GENDIV.DIV = // division factor
	GENCTRL.ID 
	*/

	//struct GENCTRL_register genctrl = {.ID = 0x01, .GENEN = 1}; // SERCOM2_CORE, 0x14 is SERCOM0_CORE
	//struct GENDIV_register gendiv = {.ID = 0x01, .DIV = 0x8};

	//struct GCLK * gclk = (struct GCLK *)0x40000C00;

	// SET PORT DIRECTION
	//struct PortIO * port_io_ptr = port_io_init();
	//port_io_set_dir(port_io_ptr, 10, PORT_IO_OUTPUT);
	//port_io_set_dir(port_io_ptr, 11, PORT_IO_INPUT);



	//self->CTRLA &= (1<<1);
	//self->CTRLA &= (1<<2);
	//return;
	self->USART.CTRLA.bit.SWRST = 1;
	while(self->USART.CTRLA.bit.SWRST || self->USART.SYNCBUSY.bit.SWRST);
	//self->CTRLA.SWRST = 1;
	//while(self->CTRLA.SWRST || self->SYNCBUSY.SWRST); // wait for reset to complete
}

void _uart_enable(Sercom * self)
{
	// must be called very last, otherwise registers are read only
	//self->CTRLA.ENABLE = 1; // enable SERCOM
	self->USART.CTRLA.bit.ENABLE = 1;
	_uart_sync(self);
}


//PUBLIC

//struct SERCOM * uart_create(struct SERCOM * uart_addr)
// right now, this is somewhat singleton "by design" (or accident, *cough*). I should change this... Yet another TODO!
Sercom * uart_create()
{

	// TX pullup output
	PORT->Group[0].PINCFG[10].bit.PULLEN = 1;
	PORT->Group[0].PINCFG[10].bit.INEN	= 0;
	PORT->Group[0].OUTSET.reg = (1 << 10);
	PORT->Group[0].DIRCLR.reg = (1 << 10);

	// RX pullup input
	PORT->Group[0].PINCFG[11].bit.PULLEN = 1;
	PORT->Group[0].PINCFG[11].bit.INEN	= 1;
	PORT->Group[0].OUTSET.reg = (1 << 11);
	PORT->Group[0].DIRCLR.reg = (1 << 11);

	// Port Init -- TODO: possibly put this in portio driver.
	PORT->Group[0].PINCFG[10].bit.PMUXEN = 1; // TX port PA10
	PORT->Group[0].PINCFG[11].bit.PMUXEN = 1; // RX port PA11
	PORT->Group[0].PMUX[5].reg = PORT_PMUX_PMUXE_C | PORT_PMUX_PMUXO_C; // set to peripheral function C (SERCOM)


	Sercom * sercom = SERCOM0;
	
	// Turn on SERCOM 
	// TODO: turn on right SERCOM... SERCOM0!!!!
	PM->APBCMASK.reg |= (PM_APBCMASK_PAC2 | PM_APBCMASK_SERCOM0);

	// move this stuff to function
	// enable XOSC
	//SYSCTRL->XOSC.reg =
	//	SYSCTRL_XOSC_STARTUP(0x8u) // startup time (256 cycles), figure out best value here: TODO!
		//| SYSCTRL_XOSC_GAIN_4 // gain, not used since I'm trying to use ampgc
	//	| SYSCTRL_XOSC_AMPGC(1); // automatic amplitude gain control
	//SYSCTRL->XOSC.bit.ENABLE = 1;
	//while(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_XOSCRDY) == 0); // wait for the oscillator to stabilize


	// configure sercom clock to run off generic clock source 7 (DFLL48M)

	// generic clock generator 0 (system main clock) uses 48MHz DFLL
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(SERCOM0_GCLK_ID_CORE) | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(0x0); // must be single 16-bit write
	
	// reset UART before initializing
	_uart_reset(sercom);
	// initialize new UART
	_uart_init(sercom);
	// enable last
	_uart_enable(sercom); // TODO: maybe have this as a public function and call it later on manually, that way you could configure the uart differently before enabling it.
	return sercom;
}

void uart_send(Sercom * self, char data)
{
	self->USART.DATA.reg = 0xFF; // char to int
}