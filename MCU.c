
#include "include.h"
#include <htc.h>

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
    ANSELE = 0x00;
    ANSELD = 0x00;
    ANSELC = 0x00;
    ANSELB = 0x00;
    ANSELA = 0x00;

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
    TRISE2 = 0;
    TRISE1 = 0;
    TRISE0 = 0;
    TRISA4 = 0;
    TRISA5 = 0;

    TRISB4 = 0;
    TRISB5 = 0;
    TRISA0 = 0;


    
    TRISD4 = 0;
    TRISD6 = 0;
    TRISD7 = 1;

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

    // USE SWITCH
    HARD0 = 1;
    HARD1 = 1;
    BS = 1;
    RST = 1;

    MCU_SPI_SI_SEL2 = 0;
    MCU_SPI_SI_SEL1 = 0;
    MCU_SPI_SI_SEL0 = 0;

    BE_CS_S6 = 0;
    BE_CS_S5 = 0;
    BE_CS_S4 = 0;
    BE_CS_S3 = 0;

    BE_CS_S2 = 0;
    BE_CS_S1 = 0;
    BE_CS_S0 = 0;

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

void UARTWriteHex(u8 val[4]) {
    u8 temp;
    u8 i;    
    for(i=0;i<4;i++) {
       temp = val[i];
       UARTWriteOneHex(temp);
    }
}

void UARTWriteOneHex(u8 val) {
    u8 str[3] = {0, 0, 0};
    u8 alphabet[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    str[0] = alphabet[val / 16];
    str[1] = alphabet[val % 16];
    UARTWriteString(str);
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

void SPI_Write(u8 data) {
    SSP1BUF = data;
    while (PIR1bits.SSP1IF == 0);
    PIR1bits.SSP1IF = 0;
}

u8 SPI_Read() {
    SSP1BUF = 0x00;
    while (PIR1bits.SSP1IF == 0);
    PIR1bits.SSP1IF = 0;
    return (SSP1BUF);
}

void SPI_Init() {
    // SI_SEL
    TRISA1 = 0;
    TRISA2 = 0;
    TRISA3 = 0;

    TRISC3 = 0; // SCK
    TRISC4 = 1; // SDI
    TRISC5 = 0; // SDO

    SSP1STAT = 0B00000000;  // SMP|CKE|DA|P|S|RW|UA|BF

    // SPI MASTER
    SSP1CON1 = 0B00110000; //WCOL|SSPOV|SSPEN|CKP|SSPM3|SSPM2|SSPM1|SSPM0
    SSP1CON2 = 0B00000000; //GCEN|ACKSTAT|ACKDT|ACKEN|RCEN|PEN|RSEN|SEN
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







