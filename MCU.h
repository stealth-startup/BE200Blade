#define u8  unsigned char
#define u16  unsigned int
#define u32 unsigned long

// SI Sel
#define MCU_SPI_SI_SEL2 PORTAbits.RA3
#define MCU_SPI_SI_SEL1 PORTAbits.RA2
#define MCU_SPI_SI_SEL0 PORTAbits.RA1

// 3-8 Enable
#define BE_CS_S6 PORTAbits.RA4
#define BE_CS_S5 PORTAbits.RA5
#define BE_CS_S4 PORTEbits.RE0
#define BE_CS_S3 PORTEbits.RE1

// 3-8 Input
#define BE_CS_S2 PORTAbits.RA0
#define BE_CS_S1 PORTBbits.RB5
#define BE_CS_S0 PORTBbits.RB4

#define HARD1 PORTCbits.RC2
#define HARD0 PORTDbits.RD2
#define RST   PORTCbits.RC1
#define BS    PORTCbits.RC0

#define LED PORTEbits.RE2


void MCU_delay(void);
void MCU_INIT(void);

void TimerInit(void);
u32 GetTime(void);

void SPI_Init(void);
void SPI_Write(char data);
u8 SPI_Read();

#define RECEIVE_BUFF_SIZE 64

volatile char URBuff[RECEIVE_BUFF_SIZE];	//USART Receive Buffer
volatile signed char UQFront;
volatile signed char UQEnd;

void  UART_INIT(void);

void UARTWriteChar(char ch);
void UARTWriteString(const char *str);
void UARTWriteLine(const char *str);
void UARTWriteInt(u16 val, u8 field_length);
void UARTWriteOneHex(u8 val);
void UARTWriteHex(u8 val[4]);
void UARTGotoNewLine();

void UARTHandleRxInt();
char UARTReadData();
u8 UARTDataAvailable();
void UARTReadBuffer(char *buff,u16 len);
void UARTFlushBuffer();




