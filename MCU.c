
#include "include.h"




// PIC18F87K22 Configuration Bit Settings

// 'C' source line config statements

#include <htc.h>

#define FIFO_SIZE 64

char pbBuffer[FIFO_SIZE];
char *pbPut;
char *pbGet;

unsigned char counter = 0; //Overflow counter
unsigned char seconds = 0;

void TimerInit() {
    counter = 0;
    seconds = 0;
}

u32 GetTime() {
    return seconds * 1000 + counter * 4;
}

void MCU_delay(void) {
    unsigned int i = 60000;
    while (i--);
}

void MCU_INIT(void) {
    //TRISC6 = 0;
    TRISC5 = 0; //
    TRISC4 = 1;
    TRISD3 = 1;
    TRISD2 = 0;
    TRISD1 = 1;
    TRISD0 = 1;
    TRISC3 = 0;
    TRISC2 = 0;
    TRISC1 = 0;

    TRISC0 = 0;
    TRISA7 = 1;
    TRISE2 = 0;
    TRISE1 = 0;
    TRISE0 = 0;
    TRISA4 = 0;

    TRISB4 = 0;
    TRISB5 = 0;
    TRISA0 = 0;
    TRISA1 = 0;
    TRISA2 = 0;
    TRISA3 = 0;

    
    TRISD4 = 0;

    PSA = 0;
    T0SE = 0;
    T0CS = 0;
    T08BIT = 1;
    T0PS0 = 1; T0PS1 = 1; T0PS2 = 1;
    
    TMR0IE = 1; //Enable TIMER0 Interrupt
    PEIE = 1; //Enable Peripheral Interrupt
    GIE = 1; //Enable INTs globally
    TMR0ON = 1; //Now start the timer!

    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
    IPR1bits.CCP1IP = 1; //prioritize CCP module

    UART_INIT();
    SPI_Init();

}



/*************************************************************/
//                        UART CONFIG                        //
//                                                           //
/*************************************************************/
#define  MCU_Frequency   20000000ul
#define  UART_SPEED     115200

void UART_INIT(void) {

    UQFront = UQEnd = -1;

    TRISD7 = 1;
    ANSELDbits.ANSD7=0;
    TRISD6 = 0;

    TXSTA2 = 0B00100100; // |CSRC|TX9|TXEN|SYNC|SENDB|BRGH|TRMT|TX9D|
    RCSTA2 = 0B10010000; // |SPEN|RX9|SREN|CREN|ADDEN|FERR|OERR|RX9D|

    BAUDCON2 = 0B00001000; // |ABDOVF|RCIDL|RXDTP|TXCKP|BRG16|Dummy|WUE|ABDEN|

    SPBRGH2 = 0;
    SPBRG2 = (MCU_Frequency / UART_SPEED / 4) - 1;


    PIR3bits.TX2IF = 0;
    PIR3bits.RC2IF = 0;

    PIE3bits.RC2IE = 1;
    IPR3bits.RC2IP = 1;

    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;

}

void UARTWriteChar(char ch) {
    while (!PIR3bits.TX2IF);
    TXREG2 = ch;
}

void UARTWriteString(const char *str) {
    while (*str != '\0') {
        UARTWriteChar(*str);
        str++;
    }
}

void UARTWriteLine(const char *str) {
    UARTWriteChar('\r'); //CR
    UARTWriteChar('\n'); //LF

    UARTWriteString(str);
}

#define XOFF 19

void UARTHandleRxInt() {
    //Read the data
    char data;
    data = RCREG2;

    if (data != XOFF) {
        //Now add it to q
        if (((UQEnd == RECEIVE_BUFF_SIZE - 1) && UQFront == 0) || ((UQEnd + 1) == UQFront)) {
            //Q Full
            UQFront++;
            if (UQFront == RECEIVE_BUFF_SIZE) UQFront = 0;
        }

        if (UQEnd == RECEIVE_BUFF_SIZE - 1)
            UQEnd = 0;
        else
            UQEnd++;

        URBuff[UQEnd] = data;

        if (UQFront == -1) UQFront = 0;
    }

   
}

char UARTReadData() {
    char data;

    UARTGotoNewLine();


    //Check if q is empty
    if (UQFront == -1) {

        return 0;
    }
    data = URBuff[UQFront];

    if (UQFront == UQEnd) {
        //If single data is left
        //So empty q
        UQFront = UQEnd = -1;
    } else {
        UQFront++;

        if (UQFront == RECEIVE_BUFF_SIZE)
            UQFront = 0;
    }


    return data;
}

u8 UARTDataAvailable() {
    if (UQFront == -1) return 0;
    if (UQFront < UQEnd)
        return (UQEnd - UQFront + 1);
    else if (UQFront > UQEnd)
        return (RECEIVE_BUFF_SIZE - UQFront + UQEnd + 1);
    else
        return 1;
}

void UARTWriteInt(u16 val, u8 field_length) {
    char str[5] = {0, 0, 0, 0, 0};
    u8 i = 4, j = 0;

    //Handle negative integers
    if (val < 0) {
        UARTWriteChar('-'); //Write Negative sign
        val = val*-1; //convert to positive
    } else {
        UARTWriteChar(' ');
    }

    if (val == 0 && field_length < 1) {
        UARTWriteChar('0');
        return;
    }
    while (val) {
        str[i] = val % 10;
        val = val / 10;
        i--;
    }

    if (field_length == -1)
        while (str[j] == 0) j++;
    else
        j = 5 - field_length;


    for (i = j; i < 5; i++) {
        UARTWriteChar('0' + str[i]);
    }
}

void UARTGotoNewLine() {
    UARTWriteChar('\r'); //CR
    UARTWriteChar('\n'); //LF
}

void UARTReadBuffer(char *buff, u16 len) {
    u16 i;
    for (i = 0; i < len; i++) {
        buff[i] = UARTReadData();
    }
}

void UARTFlushBuffer() {
    while (UARTDataAvailable() > 0) {
        UARTReadData();
    }
}

void SPI_Write(char data) {
    SSPBUF = data;
    while (!SSP1STATbits.BF);
}

u8 SPI_Read() {
    SSPBUF = 0x00;
    while (!SSP1STATbits.BF);
    return (SSPBUF);
}

void SPI_Init() {
    SSPSTAT = 0x40; // Set SMP=0 and CKE=1. Notes: The lower 6 bit is read only
    SSPCON1 = 0x20;
}

u8 LED_Stat = 0;

void interrupt HighISR(void) {
    if (PIR3bits.RC2IF) {
        UARTHandleRxInt();        
        return;
    }

    if (TMR0IE && TMR0IF) {
        //TMR0 Overflow ISR
        counter++; //Increment Over Flow Counter
        if (counter == 76) {
            counter = 0;
            seconds++;
            LED_Stat^=1;
            LED = LED_Stat;
            
        }
        //Clear Flag
        TMR0IF = 0;
        return;
    }
}

void interrupt low_priority  LowISR(void)
{

}







