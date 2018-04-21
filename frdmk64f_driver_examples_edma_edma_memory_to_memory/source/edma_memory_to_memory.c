/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_edma.h"
#include "fsl_dmamux.h"

#include "pin_mux.h"
#include "clock_config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
//#define TRANSFER_ROM_TO_RAM
#define TRANSFER_RAM_TO_RAM

#define EXAMPLE_DMA DMA0
#define EXAMPLE_DMAMUX DMAMUX0
#define DMA_CH0 0
#define DMA_CH1 1
#define DMA_CH2 2
#define DMA_CH3 3
#define TRANSFER_MODE 
#define BUFF_LENGTH 8192UL

/* Systick Counter */
#define GET_TICKS() (0xFFFFFF - SysTick->VAL)
#define STOP_COUNTING() (SysTick->CTRL &= (~SysTick_CTRL_ENABLE_Msk))


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void SysTick_Handler(void);
/*******************************************************************************
 * Variables
 ******************************************************************************/
/*DMA Variables*/
edma_handle_t* g_EDMA_Handle[4];
edma_handle_t g_EDMA_Handle_ch0; //for the test of 1byte(8bits) transfer
edma_handle_t g_EDMA_Handle_ch1; //for the test of 4byte(32bits) transfer
edma_handle_t g_EDMA_Handle_ch2; //for the test of 16byte transfer
edma_handle_t g_EDMA_Handle_ch3; //for the test of 32byte transfer

edma_transfer_config_t transferConfig;
edma_config_t userConfig;
uint32_t dma_start,dma_end;
__attribute__ ((section (".data.$SRAM_LOWER") )) volatile bool g_Transfer_Done = false;
extern char _binary_randomData_bin_start[]; //Source addr in case of ROM-to-RAM transfer 
uint32_t transferByte; 

#if defined TRANSFER_RAM_TO_RAM
__attribute__ ((section(".data.SRAM_UPPER"))) __attribute__ ((aligned(32))) uint8_t srcAddr[BUFF_LENGTH]={0}; //source table array for RAM-to-RAM
#else defined TRANSFER_ROM_TO_RAM
void* srcAddr; //Source addr for DMA
#endif
__attribute__ ((section(".data.SRAM_UPPER"))) __attribute__ ((aligned(32))) uint8_t destAddr[BUFF_LENGTH] = {0};


/*DMA Throughput counter*/
volatile uint32_t cnt;
volatile uint32_t ret;
double coreClock;
double scalingFactor;
uint32_t result;
uint32_t SysIsrcount=0;

/*******************************************************************************
 * Code
 ******************************************************************************/

/* User callback function for EDMA transfer. */
void EDMA_Callback(edma_handle_t *handle, void *param, bool transferDone, uint32_t tcds)
{
    
    dma_end = GET_TICKS();
    STOP_COUNTING();
    
    if (transferDone)
    {
        g_Transfer_Done = true;
    }
}

void SysTick_Handler(void){

	SysIsrcount++;
}

__attribute__ ((long_call, section (".ramfunc.$SRAM_LOWER") )) void dma_polling(void) 
{
    /* Wait for EDMA transfer finish */
    while (g_Transfer_Done != true)
    {
    }
}


/*!
 * @brief Main function
 */
int main(void)
{
    uint32_t i = 0;
    
    
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    /* Print source buffer */
    PRINTF("EDMA memory to memory transfer example begin.\r\n\r\n");

    /* For RAM-to-RAM transfer test
     * Prepare source data */
#if defined TRANSFER_RAM_TO_RAM
    for (uint32_t i = 0; i < BUFF_LENGTH; i++)
    {
        srcAddr[i] = i;
    }
#else defined TRANSFER_ROM_TO_RAM
    srcAddr = _binary_randomData_bin_start;
#endif 


    /* Configure DMAMUX */
    DMAMUX_Init(EXAMPLE_DMAMUX);
#if defined(FSL_FEATURE_DMAMUX_HAS_A_ON) && FSL_FEATURE_DMAMUX_HAS_A_ON
    DMAMUX_EnableAlwaysOn(EXAMPLE_DMAMUX, 0, true);
#else
    for (uint8_t i=0; i<4;i++){
        DMAMUX_SetSource(EXAMPLE_DMAMUX, i, 63);
    }
#endif /* FSL_FEATURE_DMAMUX_HAS_A_ON */
    for (uint8_t i=0; i<4;i++){
        DMAMUX_EnableChannel(EXAMPLE_DMAMUX, i);
    }
    /* Configure EDMA one shot transfer */
    /*
     * userConfig.enableRoundRobinArbitration = false;
     * userConfig.enableHaltOnError = true;
     * userConfig.enableContinuousLinkMode = false;
     * userConfig.enableDebugMode = false;
     */
    

    EDMA_GetDefaultConfig(&userConfig);
    EDMA_Init(EXAMPLE_DMA, &userConfig);
    /*DMA handle creation*/
    EDMA_CreateHandle(&g_EDMA_Handle_ch0, EXAMPLE_DMA, DMA_CH0);
    EDMA_CreateHandle(&g_EDMA_Handle_ch1, EXAMPLE_DMA, DMA_CH1);
    EDMA_CreateHandle(&g_EDMA_Handle_ch2, EXAMPLE_DMA, DMA_CH2);
    EDMA_CreateHandle(&g_EDMA_Handle_ch3, EXAMPLE_DMA, DMA_CH3);
        
    g_EDMA_Handle[0] = &g_EDMA_Handle_ch0;
    g_EDMA_Handle[1] = &g_EDMA_Handle_ch1;
    g_EDMA_Handle[2] = &g_EDMA_Handle_ch2;
    g_EDMA_Handle[3] = &g_EDMA_Handle_ch3;

    

    for (uint8_t i=0; i<4;i++){ //loop while ch0 - ch3
        EDMA_SetCallback(g_EDMA_Handle[i], EDMA_Callback, NULL);
        switch (i){
            case 0:
                transferByte = (uint32_t) 1; //1Byte transfer
                break;
            case 1:
                transferByte = (uint32_t) 4; //4Byte transfer
                break;
            case 2:
                transferByte = (uint32_t) 16; //16Byte transfer 
                break;
            case 3:
                transferByte = (uint32_t) 32; //32Byte transfer
                break;
            default:
                break;
        }
        EDMA_PrepareTransfer(&transferConfig, srcAddr, transferByte, destAddr, transferByte,
                             (uint32_t)(BUFF_LENGTH), (uint32_t)(BUFF_LENGTH*512), kEDMA_MemoryToMemory);
                
        EDMA_SubmitTransfer(g_EDMA_Handle[i], &transferConfig);
        EDMA_SetModulo(EXAMPLE_DMA, g_EDMA_Handle[i]->channel, kEDMA_Modulo8Kbytes, kEDMA_Modulo8Kbytes);


        ret= SysTick_Config(0xFFFFFF);/*<---- Here starts the cycle count */
        
        if(ret){
            PRINTF("SysTick configuration is failed.\n\r");
            while(1);
        }
    
        dma_start = GET_TICKS();
        g_Transfer_Done = false;
        SysIsrcount = 0;
        EDMA_StartTransfer(g_EDMA_Handle[i]);
        
        dma_polling(); //Polling the DMA transfer complete status
        
        cnt = dma_end - dma_start;
        cnt += (SysIsrcount*0xFFFFFF);
        coreClock = CLOCK_GetCoreSysClkFreq();
        scalingFactor = (double)cnt/coreClock;
        result=(BUFF_LENGTH*512/scalingFactor)/(1024*1024);//Unit is [MB/Sec]
        
        /* Print out result */
        PRINTF("DMA throughput (%dByte transfer) is %d MB/Sec\r\n",transferByte ,result);          

    }
    
    PRINTF("\r\n\r\nEDMA memory to memory transfer example finish.\r\n\r\n");
    
    while (1)
    {
    }
}
