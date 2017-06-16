/*
* UART.c
*
* Created: 6/12/2017 8:29:27 PM
*  Author: Patrick Rohr
*/ 

#include "UART.h"
#include "PortIO.h"
//#include "samd21g18a.h" // do not include this anymore, leaving this here for reference purposes
#include "PortMapping.h"




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

void _uart_init(struct SERCOM * self)
{
/*
	self->CTRLA.MODE = 0x1; // configure USART with external clock
	self->CTRLA.CMODE = 0; // communication mode -> asynchronous, cause doofi would use synchronous
	self->CTRLA.RXPO = SERCOM_PIN_0_1_RX; // sercom pad to use for receiver
	self->CTRLA.TXPO = SERCOM_PIN_0_1_TX; // sercom pad to use for transmitter
	self->CTRLB.CHSIZE = 0x0; // character size -> 8 bits
	self->CTRLA.DORD = 0; // data order -> MSB first
	self->CTRLA.FORM = 0x0; // frame format ->  USART frame (no parity bit)
	self->CTRLB.SBMODE = 0; // number of stop bits to be used -> 1
	self->BAUD = 9600; // baud rate to be used
	self->CTRLB.RXEN = 1; // receiver enable
	self->CTRLB.TXEN = 1; // transmitter enable
	self->INTENSET.TXC = 1;
	*/
}

void _uart_reset(struct SERCOM * self)
{
	// debugging...
	/*
	GENDIV.ID = // gen clock generator that will be selected
	GENDIV.DIV = // division factor
	GENCTRL.ID 
	*/

	struct GENCTRL_register genctrl = {.ID = 0x01, .GENEN = 1}; // SERCOM2_CORE, 0x14 is SERCOM0_CORE
	struct GENDIV_register gendiv = {.ID = 0x01, .DIV = 0x8};

	struct GCLK * gclk = (struct GCLK *)0x40000C00;

	struct PortIO * port_io_ptr = port_io_init();
	port_io_set_dir(port_io_ptr, 10, PORT_IO_OUTPUT);
	port_io_set_dir(port_io_ptr, 11, PORT_IO_INPUT);



	self->CTRLA &= (1<<1);
	self->CTRLA &= (1<<2);
	return;
	//self->CTRLA.SWRST = 1;
	//while(self->CTRLA.SWRST || self->SYNCBUSY.SWRST); // wait for reset to complete
}

void _uart_enable(struct SERCOM * self)
{
	// must be called very last, otherwise registers are read only
	//self->CTRLA.ENABLE = 1; // enable SERCOM
}


//PUBLIC

struct SERCOM * uart_create(struct SERCOM * uart_addr)
{
	// reset UART before initializing
	_uart_reset(uart_addr);
	// initialize new UART
	_uart_init(uart_addr);
	// enable last
	_uart_enable(uart_addr); // TODO: maybe have this as a public function and call it later on manually, that way you could configure the uart differently before enabling it.
	return (struct SERCOM *) uart_addr;
}

void uart_send(struct SERCOM * self, char data)
{
	self->DATA.DATA = 0x41; // char to int
}