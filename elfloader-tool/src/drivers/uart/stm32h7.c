/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2020, HENSOLDT Cyber GmbH
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <devices_gen.h>
#include <drivers/common.h>
#include <drivers/uart.h>

#include <elfloader_common.h>

#define USART_CR1 0x00
#define USART_CR2 0x04
#define USART_CR3 0x08
#define USART_BRR 0x0C
#define USART_GTPR 0x10
#define USART_RTOR 0x14
#define USART_RQR 0x18
#define USART_ISR 0x1C
#define USART_ICR 0x20
#define USART_RDR 0x24
#define USART_TDR 0x28
#define USART_PRESC 0x2C
#define USART_HWCFGR2 0x3EC
#define USART_HWCFGR1 0x3F0
#define USART_VERR 0x3F4
#define USART_IPIDR 0x3F8
#define USART_SIDR 0x3FC

#define USART_ISR_TXE BIT(7)
#define USART_ISR_RXNE BIT(5)

#define USART_REG(mmio, x) ((volatile uint32_t *)(mmio + (x)))

static int stm32h7_uart_putchar(struct elfloader_device *dev, unsigned int c) {
	volatile void *mmio = dev->region_bases[0];

	/* Wait to be able to transmit. */
	while (!(*USART_REG(mmio, USART_ISR) & USART_ISR_TXE))
		;

	/* Transmit. */
	*USART_REG(mmio, USART_TDR) = c;

	return 0;
}

static int stm32h7_uart_init(struct elfloader_device *dev,
                             UNUSED void *match_data) {
	uart_set_out(dev);
	return 0;
}

static const struct dtb_match_table stm32h7_uart_matches[] = {
    {.compatible = "st,stm32h7-uart"},
    {.compatible = NULL /* sentinel */},
};

static const struct elfloader_uart_ops stm32h7_uart_ops = {
    .putc = &stm32h7_uart_putchar,
};

static const struct elfloader_driver stm32h7_uart = {
    .match_table = stm32h7_uart_matches,
    .type = DRIVER_UART,
    .init = &stm32h7_uart_init,
    .ops = &stm32h7_uart_ops,
};

ELFLOADER_DRIVER(stm32h7_uart);
