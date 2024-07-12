/*
 * bgt60tr13c.c
 *
 *  Created on: 31 May 2024
 *      Author: jorda
 *
 * Rutronik Elektronische Bauelemente GmbH Disclaimer: The evaluation board
 * including the software is for testing purposes only and,
 * because it has limited functions and limited resilience, is not suitable
 * for permanent use under real conditions. If the evaluation board is
 * nevertheless used under real conditions, this is done at one’s responsibility;
 * any liability of Rutronik is insofar excluded
 */

#include "bgt60tr13c.h"

// Needed for communication with BGT60TR13C (over SPI)
#include "cyhal_spi.h"
#include "cycfg_pins.h"

#include "xensiv_bgt60trxx_mtb.h"

#define XENSIV_BGT60TRXX_CONF_IMPL
#include "radar_settings.h"

#define NUM_SAMPLES_PER_FRAME               (XENSIV_BGT60TRXX_CONF_NUM_RX_ANTENNAS *\
                                             XENSIV_BGT60TRXX_CONF_NUM_CHIRPS_PER_FRAME *\
                                             XENSIV_BGT60TRXX_CONF_NUM_SAMPLES_PER_CHIRP)

#define NUM_SAMPLES_PER_CHIRP				XENSIV_BGT60TRXX_CONF_NUM_SAMPLES_PER_CHIRP


/**
 * Handle to the SPI communication block. Enables to communicate over SPI with the BGT60TR13C IC.
 */
static cyhal_spi_t spi_obj;

/**
 * Handle to the BGT60TR13C IC. Enables to configure it and to read out its values.
 */
static xensiv_bgt60trxx_mtb_t sensor;

static uint16_t data_available = 0;

/**
 * @brief Initializes the SPI communication with the radar sensor
 *
 * @retval 0 Success
 * @retval -1 Error occurred during initialization
 * @retval -2 Cannot change the frequency
 */
static int init_spi()
{
	if (cyhal_spi_init(&spi_obj,
			ARDU_MOSI,
			ARDU_MISO,
			ARDU_CLK,
			NC,
			NULL,
			8,
			CYHAL_SPI_MODE_00_MSB,
			false) != CY_RSLT_SUCCESS)
	{
		return -1;
	}

	// Set the data rate to spi_freq Mbps
	if (cyhal_spi_set_frequency(&spi_obj, 12500000UL) != CY_RSLT_SUCCESS)
	{
		return -2;
	}

	return 0;
}

#if defined(CYHAL_API_VERSION) && (CYHAL_API_VERSION >= 2)
void xensiv_bgt60trxx_mtb_interrupt_handler(void *args, cyhal_gpio_event_t event)
#else
void xensiv_bgt60trxx_mtb_interrupt_handler(void *args, cyhal_gpio_irq_event_t event)
#endif
{
    CY_UNUSED_PARAMETER(args);
    CY_UNUSED_PARAMETER(event);

    // Values are available, then can be read using the function xensiv_bgt60trxx_get_fifo_data
    data_available = 1;
}

int bgt60tr13c_init()
{
	cy_rslt_t result = CY_RSLT_SUCCESS;

	if (init_spi() != 0) return -1;

	/*Initialize BGT60TR13C Power Control pin*/
	result = cyhal_gpio_init(ARDU_IO3, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, true); /*Turn it ON*/

	if (result != CY_RSLT_SUCCESS) return -2;

	/*Initialize NJR4652F2S2 POWER pin*/
	result = cyhal_gpio_init(ARDU_IO7, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_OPENDRAINDRIVESLOW, false); /*Keep it OFF*/
	if (result != CY_RSLT_SUCCESS) return -2;

	/*Must wait at least 1ms until the BGT60TR13C sensor power supply gets to nominal value*/
	CyDelay(10);

	result = xensiv_bgt60trxx_mtb_init(&sensor,
			&spi_obj,
			ARDU_CS,
			ARDU_IO4,
			register_list,
			XENSIV_BGT60TRXX_CONF_NUM_REGS);

	if (result != CY_RSLT_SUCCESS) return -3;

	// The sensor will generate an interrupt once the sensor FIFO level is NUM_SAMPLES_PER_FRAME
	result = xensiv_bgt60trxx_mtb_interrupt_init(&sensor,
			NUM_SAMPLES_PER_FRAME,
			ARDU_IO6,
			CYHAL_ISR_PRIORITY_DEFAULT,
			xensiv_bgt60trxx_mtb_interrupt_handler,
			NULL);

	if (result != CY_RSLT_SUCCESS) return -4;

	if (xensiv_bgt60trxx_start_frame(&sensor.dev, true) != XENSIV_BGT60TRXX_STATUS_OK) return -6;

	return 0;
}

uint16_t bgt60tr13c_is_data_available()
{
	if (data_available != 0)
	{
		data_available = 0;
		return 1;
	}
	return 0;
}

uint16_t bgt60tr13c_get_samples_per_frame()
{
	return NUM_SAMPLES_PER_FRAME;
}

uint16_t bgt60tr13c_get_antenna_count()
{
	return XENSIV_BGT60TRXX_CONF_NUM_RX_ANTENNAS;
}

uint16_t bgt60tr13c_get_chirps_per_frame()
{
	return XENSIV_BGT60TRXX_CONF_NUM_CHIRPS_PER_FRAME;
}

uint16_t bgt60tr13c_get_samples_per_chirp()
{
	return XENSIV_BGT60TRXX_CONF_NUM_SAMPLES_PER_CHIRP;
}

uint64_t bgt60tr13c_get_end_freq()
{
	return XENSIV_BGT60TRXX_CONF_END_FREQ_HZ;
}

uint64_t bgt60tr13c_get_start_freq()
{
	return XENSIV_BGT60TRXX_CONF_START_FREQ_HZ;
}

uint32_t bgt60tr13c_get_sampling_rate()
{
	return XENSIV_BGT60TRXX_CONF_SAMPLE_RATE;
}

int bgt60tr13c_get_data(uint16_t* data)
{
	// Get the FIFO data
	int32_t retval = xensiv_bgt60trxx_get_fifo_data(&sensor.dev, data, NUM_SAMPLES_PER_FRAME);
	if (retval == XENSIV_BGT60TRXX_STATUS_OK)
	{
		return 0;
	}

	// An error occurred when reading the FIFO
	// Restart the frame generation
	xensiv_bgt60trxx_start_frame(&sensor.dev, false);
	xensiv_bgt60trxx_start_frame(&sensor.dev, true);

	return -1;
}
