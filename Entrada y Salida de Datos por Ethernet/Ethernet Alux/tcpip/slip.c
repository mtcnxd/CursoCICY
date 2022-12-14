/*********************************************************************
 *
 *                  SLIP Module for Microchip TCP/IP Stack
 *                 (Modified to work with CCS PCH, by CCS)
 *
 *********************************************************************
 * FileName:        SLIP.c
 * Dependencies:    stacktsk.h
 *                  mac.h
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.10PL1 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the ?Company?) for its PICmicro? Microcontroller is intended and
 * supplied to you, the Company?s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN ?AS IS? CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     11/27/01 Original        (Rev 1.0)
 * Nilesh Rajbharti     2/9/02  Cleanup
 * Darren Rook (CCS)    01/09/04 Initial CCS Public Release
 ********************************************************************/
#include "tcpip/stacktsk.h"
#include "tcpip/mac.h"

#define MAX_SLIP_BUFFER_SIZE    MAC_TX_BUFFER_SIZE

#if (MAX_SLIP_BUFFER_SIZE > 250)
#error Invalid MAX_SLIP_BUFFER_SIZE detected
#endif

#define BAUD_RATE               (38400)     // bps

#define USART_USE_BRGH_LOW
#if defined(USART_USE_BRGH_LOW)
    #define SPBRG_VAL           ( ((getenv("CLOCK")/BAUD_RATE)/64) - 1)
#else
    #define SPBRG_VAL           ( ((getenv("CLOCK")/BAUD_RATE)/16) - 1)
#endif

#if SPBRG_VAL > 255
    #error "Calculated SPBRG value is out of range for currnet Clock speed."
#endif

typedef struct _SLIP_TX_BUFFER
{
    int8 Length;
    int8 IOAccess;
    int8 CallerAccess;
    union
    {
        struct
        {
            unsigned int bIsModemString : 1;
        } bits;
        int8 b;
    } Flags;
    int8 Data[MAX_SLIP_BUFFER_SIZE];
} SLIP_TX_BUFFER;

typedef struct _SLIP_RX_BUFFER
{
    int8 CurrentPacket;
    int8 PacketCount;
    int8 IOAccess;
    int8 CallerAccess;
    int8 Data[MAX_SLIP_BUFFER_SIZE];        // Format: <Next>[<Data>...]
} SLIP_RX_BUFFER;


SLIP_TX_BUFFER TxBuffer;
SLIP_RX_BUFFER RxBuffer;

static int1 bIsRxActive;


/*
 * SLIP escape character as per RFC 1055
 */
#define END                     (0xc0)
#define ESC                     (0xdb)
#define ESC_END                 (0xdc)
#define ESC_ESC                 (0xdd)

typedef enum _SM_SLIP
{
    SM_SLIP_SOF = 0,
    SM_MODEM_CMD,
    SM_MODEM_CMD_AT,
    SM_SLIP_DATA,
    SM_SLIP_ESC,
    SM_SLIP_OVERFLOW
} SM_SLIP;

static struct
{
    int8    tempData;
    int8    currentPacket;
    SM_SLIP smSLIP;
    int8    currentLength;
    union
    {
        struct
        {
            unsigned int bIsDataReady   : 1;
            unsigned int bIsPacketDone  : 1;
        } bits;
        int8 b;
    } Flags;
} ISRLocals;


void MACISR(void)
{
    /*
     * If we have received an character, handle it.
     */
    if ( PIR1_RCIF && PIE1_RCIE )
    {
        /*
         * Check for any error, and clear it if there is any.
         */
        if ( RCSTA & 0x06 )
        {
            RCSTA_CREN = 0;
            RCSTA_CREN = 1;
            ISRLocals.smSLIP = SM_SLIP_OVERFLOW;
        }

        ISRLocals.tempData = RCREG;

        ISRLocals.Flags.bits.bIsDataReady = FALSE;

        switch(ISRLocals.smSLIP)
        {
        default:    // case SM_SLIP_SOF:
            /*
             * Remember current packet start location in a RxBuffer.
             * This will be useful when buffer overflows and we need to
             * discard packet that was overflown, keeping previously
             * loaded packets intact.
             */
            ISRLocals.currentPacket             = RxBuffer.IOAccess;
            ISRLocals.currentLength             = 0;

            if ( ISRLocals.tempData == END )
                ISRLocals.smSLIP = SM_SLIP_DATA;

            /*
             * Handle modem commands differently.
             */
            else if ( ISRLocals.tempData == 'A' )
            {
                ISRLocals.Flags.bits.bIsDataReady = TRUE;
                ISRLocals.smSLIP = SM_MODEM_CMD_AT;
            }
            break;

        case SM_MODEM_CMD:
            ISRLocals.Flags.bits.bIsDataReady = TRUE;
            if ( ISRLocals.tempData == '\r' )
            {
                ISRLocals.Flags.bits.bIsPacketDone = TRUE;
            }
            break;

        case SM_MODEM_CMD_AT:
            ISRLocals.Flags.bits.bIsDataReady = TRUE;
            if ( ISRLocals.tempData == 'T' )
                ISRLocals.smSLIP = SM_MODEM_CMD;
            else
                ISRLocals.smSLIP = SM_SLIP_OVERFLOW;
            break;

        case SM_SLIP_DATA:
            ISRLocals.Flags.bits.bIsDataReady   = TRUE;
            /*
             * If this is a escape character, wait for next
             * character to determine its value.
             */
            if ( ISRLocals.tempData == ESC )
            {
                ISRLocals.Flags.bits.bIsDataReady = FALSE;
                ISRLocals.smSLIP = SM_SLIP_ESC;
            }

            else if ( ISRLocals.tempData == END )
            {
                /*
                 * End of packet is received.
                 * Make sure that we have at least one data byte
                 * before declaring that new packet is received.
                 */
                if ( ISRLocals.currentLength )
                {
                    ISRLocals.Flags.bits.bIsPacketDone = TRUE;

                    /*
                     * Even though we are saving END character,
                     * it will be overwritten by next packet as per
                     * bIsPacketDone logic.
                     * This saves some code space as we do not have to
                     * wrap IOAccess index twice.
                     */
                }
                else
                    ISRLocals.Flags.bits.bIsDataReady = FALSE;
            }

            break;

        case SM_SLIP_ESC:
            // Save the data.
            ISRLocals.Flags.bits.bIsDataReady   = TRUE;
            ISRLocals.smSLIP                    = SM_SLIP_DATA;

            if ( ISRLocals.tempData == ESC_END )
                ISRLocals.tempData = END;

            else if ( ISRLocals.tempData == ESC_ESC )
                ISRLocals.tempData = ESC;

            else
                /*
                 * This is an invalid character sequence.
                 * Do not save it.
                 */
            {
                ISRLocals.Flags.bits.bIsDataReady = FALSE;
                ISRLocals.smSLIP = SM_SLIP_SOF;
            }
            break;

        case SM_SLIP_OVERFLOW:
            RxBuffer.IOAccess                   = ISRLocals.currentPacket;
            ISRLocals.smSLIP                    = SM_SLIP_SOF;
            break;
        }

        if ( ISRLocals.Flags.bits.bIsDataReady )
        {
            RxBuffer.IOAccess++;
            if ( RxBuffer.IOAccess >= MAX_SLIP_BUFFER_SIZE )
                RxBuffer.IOAccess = 0;

            /*
             * Make sure that we have not reached end of buffer.
             */
            if ( RxBuffer.IOAccess != RxBuffer.CurrentPacket )
            {
                ISRLocals.currentLength++;
                RxBuffer.Data[RxBuffer.IOAccess] = ISRLocals.tempData;
            }
            else
            {
                /*
                 * We have reached end of buffer.
                 * Discard this packet by restoring IOAccess to begining
                 * of current packet.
                 */
                RxBuffer.IOAccess                   = ISRLocals.currentPacket;
                ISRLocals.smSLIP                    = SM_SLIP_SOF;
                ISRLocals.Flags.bits.bIsPacketDone  = FALSE;
            }

        }

        if ( ISRLocals.Flags.bits.bIsPacketDone )
        {
            ISRLocals.Flags.bits.bIsPacketDone      = FALSE;

            /*
             * Create link to next packet.
             */
            RxBuffer.Data[ISRLocals.currentPacket]  = RxBuffer.IOAccess;
            RxBuffer.PacketCount++;

            /*
             * Start waiting for another packet.
             */
            ISRLocals.smSLIP                        = SM_SLIP_SOF;
        }
    }

    if ( PIR1_TXIF && PIE1_TXIE )
    {
        /*
         * If there is any data, transmit it.
         */
        if ( TxBuffer.Length )
        {
            ISRLocals.tempData = TxBuffer.Data[TxBuffer.IOAccess];
            if ( ISRLocals.tempData == END )
            {
                ISRLocals.tempData = ESC;
                TxBuffer.Data[TxBuffer.IOAccess] = ESC_END;
            }
            else if ( ISRLocals.tempData == ESC )
            {
                ISRLocals.tempData = ESC;
                TxBuffer.Data[TxBuffer.IOAccess] = ESC_ESC;
            }
            else
            {
                TxBuffer.Length--;
                TxBuffer.IOAccess++;
            }
            TXREG = ISRLocals.tempData;
        }
        else
        {
            /*
             * Else, disable transmit interrupt.
             */
            PIE1_TXIE = 0;

            if ( TxBuffer.Flags.bits.bIsModemString )
                TxBuffer.Flags.bits.bIsModemString = FALSE;
            else
                TXREG = END;

        }
    }
}

void MACInit(void)
{
    MACDiscardTx(0);

    /*
     * Initialize ISR variables.
     */
    ISRLocals.smSLIP        = SM_SLIP_SOF;
    ISRLocals.Flags.b    = 0;

    RxBuffer.CurrentPacket  = 0;
    RxBuffer.PacketCount    = 0;
    RxBuffer.CallerAccess   = 0;
    RxBuffer.IOAccess       = 0;

    bIsRxActive = FALSE;

    /*
     * Clear receive interrupt flag.
     */
    PIR1_RCIF = 0;

    /*
     * Enable receive interrupt.
     * Do not enable transmit interrupt yet.  Enable it only when
     * there is something to transmit
     */
    PIE1_RCIE = 1;
    PIE1_TXIE = 0;

    /*
     * Configure USART for Asynch, 8-bit mode.
     */
#if defined(USART_USE_BRGH_LOW)
    TXSTA = 0b00100000;
#else
    TXSTA = 0b00100100;
#endif

    RCSTA = 0b10010000;
    SPBRG = SPBRG_VAL;

    /*
     * Enable global and peripheral interrupts.
     */
    //INTCON_GIEH = 1;
    //INTCON_GIEL = 1;
    enable_interrupts(GLOBAL);

}

int1    MACIsTxReady(void)
{
    return ( (TxBuffer.Length < MAX_SLIP_BUFFER_SIZE) && !PIE1_TXIE);
}

int1    MACGetHeader(MAC_ADDR *remote, int8* type)
{
    /*
     * This marks that all future accesses to MACGet and MACPut
     * be applied to Receive buffer.
     */
    bIsRxActive = TRUE;

    if ( RxBuffer.PacketCount )
    {

        /*
         * Set up packet access index.
         */
        RxBuffer.CallerAccess = RxBuffer.CurrentPacket;

        RxBuffer.CallerAccess++;
        if ( RxBuffer.CallerAccess >= MAX_SLIP_BUFFER_SIZE )
            RxBuffer.CallerAccess = 0;


        /*
         * Handle modem commands differently.
         */
        if ( RxBuffer.Data[RxBuffer.CallerAccess] == 'A' )
        {
            /*
             * Once a modem command is detected, we are not interested
             * in detail.
             */
            MACDiscardRx();

            /*
             * Mark TxBuffer for Modem String.
             * This will make sure that Transmit ISR does nottransmit END
             * at the end.
             */
            TxBuffer.Flags.bits.bIsModemString = TRUE;

            /*
             * Since this special handling does not follow standard
             * SLIP buffer logic, we will setup all required variables
             * manually.
             */
            TxBuffer.Length = 4;
            TxBuffer.CallerAccess = 0;

            /*
             * Remember to use transmit buffer for MACPut
             */
            bIsRxActive = FALSE;

            /*
             * Now load modem response.
             */
            MACPut('O');
            MACPut('K');
            MACPut('\r');
            MACPut('\n');

            /*
             * Transmit it.
             */
            MACFlush();
        }
        else
        {
            /*
             * This was not a modem command.
             * It must be IP packet.  Mark it accordingly and return.
             */
            *type = MAC_IP;

            return TRUE;
        }
    }
    return FALSE;
}

int8    MACGet(void)
{
    int8 c;

    /*
     * Depending on what buffer is active, act accordingly.
     */
    if ( bIsRxActive )
    {
        c = RxBuffer.Data[RxBuffer.CallerAccess++];
        if ( RxBuffer.CallerAccess >= MAX_SLIP_BUFFER_SIZE )
            RxBuffer.CallerAccess = 0;
    }
    else
    {
        /*
         * TxBuffer.Data contains data bytes only. There is no link byte.
         */
        c = TxBuffer.Data[TxBuffer.CallerAccess++];
    }

    return c;
}

int16    MACGetArray(int8 *val, int16 len)
{
    /*
     * This function does not verify requested bytes against available
     * bytes in current packet.  Higher level logic must always validate
     * packet and fetch data according to packet header.
     */

    while( len-- )
        *val++ = MACGet();
    return len;
}

void    MACDiscardRx(void)
{
    PIE1_RCIE = 0;

    if ( RxBuffer.PacketCount )
    {
        /*
         * Once a packet is discarded, current packet becomes next one.
         */
        RxBuffer.CurrentPacket    = RxBuffer.Data[RxBuffer.CurrentPacket];

        RxBuffer.PacketCount--;
    }

    PIE1_RCIE = 1;
}

void    MACPutHeader(MAC_ADDR *remote,
                     int8 type,
                     int16 dataLen)
{
    TxBuffer.Length         = dataLen;
    TxBuffer.CallerAccess   = 0;
    bIsRxActive             = FALSE;
}

void    MACPut(int8 val)
{
    if ( bIsRxActive )
    {
        RxBuffer.Data[RxBuffer.CallerAccess++] = val;
        if ( RxBuffer.CallerAccess >= MAX_SLIP_BUFFER_SIZE )
            RxBuffer.CallerAccess = 0;
    }
    else
    {
        TxBuffer.Data[TxBuffer.CallerAccess++] = val;
    }
}

void    MACPutArray(int8 *val, int16 len)
{
    while( len-- )
        MACPut(*val++);
}


void    MACFlush(void)
{
    TxBuffer.CallerAccess   = 0;
    TxBuffer.IOAccess       = 0;

    if ( !TxBuffer.Flags.bits.bIsModemString )
        TXREG = END;

    PIE1_TXIE = 1;
}

void    MACDiscardTx(BUFFER buff)
{
    /*
     * If there is no transmission in progress,
     * discard current buffer.
     * This is different than ethernet controller where
     * discarding transmit buffer does not affect
     * transmission that is already in progress.
     */
    if ( !PIE1_TXIE )
    {
        TxBuffer.Length         = 0;
        TxBuffer.CallerAccess   = 0;
        TxBuffer.IOAccess       = 0;
        TxBuffer.Flags.b     = 0;
    }
}

void    MACSetRxBuffer(int16 offset)
{
    /*
     * Set absolute offset.
     */
    offset += RxBuffer.CurrentPacket;

    /*
     * Skip "next packet" byte.
     */
    offset++;

    /*
     * Wrap the offset if necessary.
     */
    if ( offset >= (int8)(MAX_SLIP_BUFFER_SIZE-1) )
        RxBuffer.CallerAccess = (int8)(offset - (int8)MAX_SLIP_BUFFER_SIZE);
    else
        RxBuffer.CallerAccess = (int8)offset;

    bIsRxActive = TRUE;
}

void    MACSetTxBuffer(BUFFER buff, int16 offset)
{
    TxBuffer.CallerAccess = offset;
    bIsRxActive = FALSE;
}

void    MACReserveTxBuffer(BUFFER buff)
{
}

int16    MACGetFreeRxSize(void)
{
    int8 temp;

    PIE1_RCIE = 0;
    temp = RxBuffer.IOAccess;
    PIE1_RCIE = 1;

    if ( RxBuffer.PacketCount )
    {
        if( temp > RxBuffer.CurrentPacket )
            return ((MAX_SLIP_BUFFER_SIZE -temp) + RxBuffer.CurrentPacket);
        else
            return (RxBuffer.CurrentPacket - temp);
    }
    else
        return (MAX_SLIP_BUFFER_SIZE-2);
}















