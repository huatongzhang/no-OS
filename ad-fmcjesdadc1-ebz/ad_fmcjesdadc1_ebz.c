/***************************************************************************//**
 *   @file   ad_fmcjesdadc1_ebz.c
 *   @brief  Implementation of Main Function.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2014(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <xil_printf.h>
#include "xparameters.h"
#include "ad9517.h"
#include "ad9250.h"
#include "jesd204b_gt.h"
#include "jesd204b_v51.h"
#include "adc_core.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#ifdef _XPARAMETERS_PS_H_
#define SPI_DEVICE_ID		XPAR_PS7_SPI_0_DEVICE_ID
#define ADC_DDR_BASEADDR	XPAR_DDR_MEM_BASEADDR + 0x800000
#else
#define SPI_DEVICE_ID		XPAR_SPI_0_DEVICE_ID
#define ADC_DDR_BASEADDR	XPAR_AXI_DDR_CNTRL_BASEADDR + 0x800000
#endif

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	jesd204b_gt_state jesd204b_gt_st;
	jesd204b_state jesd204b_st;
	uint32_t jesd204b_gt_version;
	uint32_t config_reg;
	uint32_t num_of_config_regs;

	ad9517_setup(SPI_DEVICE_ID, 0);
	ad9250_setup(SPI_DEVICE_ID, 0, 0);
	ad9250_setup(SPI_DEVICE_ID, 0, 1);

	jesd204b_gt_st.num_of_lanes = 4;
	jesd204b_gt_st.use_cpll = 1;
	jesd204b_gt_st.rx_sys_clk_sel = 0;
	jesd204b_gt_st.rx_out_clk_sel = 2;
	jesd204b_gt_st.tx_sys_clk_sel = 0;
	jesd204b_gt_st.tx_out_clk_sel = 2;
	jesd204b_gt_setup(XPAR_AXI_AD9250_GT_BASEADDR, jesd204b_gt_st);

	jesd204b_st.lanesync_enable = 1;
	jesd204b_st.scramble_enable = 1;
	jesd204b_st.sysref_always_enable = 0;
	jesd204b_st.frames_per_multiframe = 32;
	jesd204b_st.bytes_per_frame = 1;
	jesd204b_st.subclass = 1;
	jesd204b_setup(XPAR_AXI_AD9250_JESD_BASEADDR, jesd204b_st);

	jesd204b_gt_read(JESD204B_GT_REG_VERSION, &jesd204b_gt_version);
	if(JESD204B_GT_VERSION_MAJOR(jesd204b_gt_version) < 7)
		num_of_config_regs = 1;
	else
		num_of_config_regs = jesd204b_gt_st.num_of_lanes;

	for (config_reg = 0; config_reg < num_of_config_regs; config_reg++)
		jesd204b_gt_clk_enable(JESD204B_GT_RX, config_reg);

	jesd204b_gt_clk_synchronize(JESD204B_GT_RX, 0);

	adc_setup(XPAR_AXI_AD9250_0_CORE_BASEADDR, XPAR_AXI_AD9250_0_DMA_BASEADDR, 2);

	adc_setup(XPAR_AXI_AD9250_1_CORE_BASEADDR, XPAR_AXI_AD9250_1_DMA_BASEADDR, 2);

	xil_printf("Done.\n");

	return 0;
}
