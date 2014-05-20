#include "include.h"
#include <string.h>
#include <stdio.h>

#include <htc.h>

//#pragma config CONFIG1H = 0x25
__CONFIG(1, FOSC_ECHPIO6 & PLLCFG_OFF & PRICLKEN_ON & FCMEN_OFF & IESO_OFF);
//#pragma config CONFIG2L = 0x19
__CONFIG(2, PWRTEN_OFF & BOREN_OFF & BORV_190);
//#pragma config CONFIG2H = 0x3F
__CONFIG(3, WDTEN_ON & WDTPS_32768);
//#pragma config CONFIG3H = 0xBC
__CONFIG(4, CCP2MX_PORTB3 & PBADEN_OFF & CCP3MX_PORTB5 & HFOFST_ON & T3CMX_PORTC0 & P2BMX_PORTD2 & MCLRE_EXTMCLR);
//#pragma config CONFIG4L = 0x85
__CONFIG(5, STVREN_ON & LVP_ON & XINST_OFF);
//#pragma config CONFIG5L = 0xF
__CONFIG(6, CP0_OFF & CP1_OFF & CP2_OFF & CP3_OFF);
//#pragma config CONFIG5H = 0xC0
__CONFIG(7, CPB_OFF & CPD_OFF);
//#pragma config CONFIG6L = 0xF
__CONFIG(8, WRT0_OFF & WRT1_OFF & WRT2_OFF & WRT3_OFF);
//#pragma config CONFIG6H = 0xE0
__CONFIG(9, WRTC_OFF & WRTB_OFF & WRTD_OFF);
//#pragma config CONFIG7L = 0xF
__CONFIG(10, EBTR0_OFF & EBTR1_OFF & EBTR2_OFF & EBTR3_OFF);
//#pragma config CONFIG7H = 0x40
__CONFIG(11, EBTRB_OFF);

unsigned char test_vector[] = {0x02, 0x00, 0x00, 0x00, 0x73, 0xba, 0x60, 0x0f, 0xbb, 0x40, 0x71, 0xa6, 0xd8, 0x72, 0x12, 0x5e, 0x92, 0x7b, 0xfe, 0x09, 0xb8, 0xfe, 0x39, 0x67, 0xee, 0xe1, 0x37, 0x41, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6a, 0x33, 0x33, 0xdd, 0x8f, 0x66, 0xfc, 0xc4, 0xb2, 0x09, 0x0a, 0x93, 0xe9, 0x17, 0x83, 0xcd, 0xc3, 0xb7, 0xc5, 0xce, 0x87, 0xf7, 0x48, 0x96, 0xe4, 0x2e, 0xe0, 0xbb, 0xde, 0xed, 0x44, 0x95, 0x99, 0x8b, 0x90, 0x52, 0xfb, 0x0b, 0x07, 0x19, 0x00, 0x00, 0x03, 0xde};

extern void sha256_init(void);
extern void sha256_write(unsigned char);
extern unsigned char* sha256_result(void);
void OpenASIC(u8);
void CloseASIC();
void Menu();
void SHA256_Test();
void CS_Test();


void OpenASIC(u8 n) {
    static u8 cs_table[32][10] = {
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
        {0, 1, 0, 0, 0, 0, 1, 0, 1, 0},
        {0, 1, 1, 0, 0, 0, 1, 0, 1, 1},
        {0, 0, 0, 0, 0, 0, 1, 1, 0, 0},
        {0, 0, 1, 0, 0, 0, 1, 1, 0, 1},
        {0, 1, 0, 0, 0, 0, 1, 1, 1, 0},
        {0, 1, 1, 0, 0, 0, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 1, 0, 0, 0, 1},
        {0, 1, 0, 0, 0, 1, 0, 0, 1, 0},
        {0, 1, 1, 0, 0, 1, 0, 0, 1, 1},
        {0, 0, 0, 0, 0, 1, 0, 1, 0, 0},
        {0, 0, 1, 0, 0, 1, 0, 1, 0, 1},
        {0, 1, 0, 0, 0, 1, 0, 1, 1, 0},
        {0, 1, 1, 0, 0, 1, 0, 1, 1, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {1, 0, 1, 0, 1, 0, 0, 0, 0, 1},
        {1, 1, 0, 0, 1, 0, 0, 0, 1, 0},
        {1, 1, 1, 0, 1, 0, 0, 0, 1, 1},
        {1, 0, 0, 0, 1, 0, 0, 1, 0, 0},
        {1, 0, 1, 0, 1, 0, 0, 1, 0, 1},
        {1, 1, 0, 0, 1, 0, 0, 1, 1, 0},
        {1, 1, 1, 0, 1, 0, 0, 1, 1, 1},
        {1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {1, 0, 1, 1, 0, 0, 0, 0, 0, 1},
        {1, 1, 0, 1, 0, 0, 0, 0, 1, 0},
        {1, 1, 1, 1, 0, 0, 0, 0, 1, 1},
        {1, 0, 0, 1, 0, 0, 0, 1, 0, 0},
        {1, 0, 1, 1, 0, 0, 0, 1, 0, 1},
        {1, 1, 0, 1, 0, 0, 0, 1, 1, 0},
        {1, 1, 1, 1, 0, 0, 0, 1, 1, 1}
    };
    u8 *csN;
    csN = cs_table[n-1];
    MCU_SPI_SI_SEL2 = csN[0];
    MCU_SPI_SI_SEL1 = csN[1];
    MCU_SPI_SI_SEL0 = csN[2];

    BE_CS_S6 = csN[3];
    BE_CS_S5 = csN[4];
    BE_CS_S4 = csN[5];

    BE_CS_S3 = csN[6];
    BE_CS_S2 = csN[7];
    BE_CS_S1 = csN[8];
    BE_CS_S0 = csN[9];
}

void CloseASIC() {
    BE_CS_S6 = 0;
    BE_CS_S5 = 0;
    BE_CS_S4 = 0;
}

void Menu(){
    
    UARTGotoNewLine();
    UARTWriteLine("Welcome");
    UARTWriteLine("h:Show Menu");
    UARTWriteLine("s:SHA256 Benchmark");
    UARTWriteLine("c:Chip Select Test(c01,c02,...c32)");
    
}

void SHA256_Test() {
    u8 i,k;
    u8 *hash;
    u8 hash1[32];
    u8 output[65];

    u32 t1, t2;
    UARTWriteLine("Start SHA256 Test...");
    TimerInit();

    t1 = GetTime();

    for (k = 0; k < 100; k++) {
        sha256_init();
        for (i = 0; i < 80; i++) sha256_write(test_vector[i]);
        hash = sha256_result();
        memcpy(hash1, hash, 32);

        sha256_init();
        for (i = 0; i < 32; i++) sha256_write(hash1[i]);
        hash = sha256_result();
        memcpy(hash1, hash, 32);
    }

    t2 = GetTime();
  
    for (i = 0; i < 32; i++) {
        sprintf(output + i * 2, "%02X", hash1[i]);
    }
    output[64] = '\0';
    UARTWriteLine("SHA256 Result:");
    UARTWriteString(output);
    sprintf(output, "%u", t2 - t1);
    UARTWriteLine("100 DHash(ms):");
    UARTWriteString(output);
}

void CS_Test() {
    char a,b;
    u8 n;

    while(UARTDataAvailable()<2);
   
    a = UARTReadData();  
    b = UARTReadData();
    n=(a-'0')*10+(b-'0');
    UARTWriteLine("Chip Select:");
    UARTWriteInt(n,2);

  
    OpenASIC(n);
    CloseASIC();
}

void main(void) {
    u8 i = 0;
    char c;
    MCU_INIT();
    LED=0;
    while (1) {
        if(UARTDataAvailable()>0) c = UARTReadData();
        switch (c) {
            case 'h':
                Menu();
                break;
            case 's':
                SHA256_Test();
                Menu();
                break;
            case 'c':
                CS_Test();
                Menu();
                break;
        }
        c=0;
    }

}


































