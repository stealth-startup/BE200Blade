#include "be200.h"

void SetPll(u8 val) {
    SPI_Write(CMD_WRITE_REG(REG_PLL));
    SPI_Write(val | 0x80);
    SPI_Write(CMD_WRITE_REG(REG_PLL));
    SPI_Write(val);
    MCU_delay();
}

void Set_BS(void) { BS = 0; }
void Clr_BS(void) { BS = 1; }


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

    MCU_delay();

    BE_CS_S6 = csN[3];
    BE_CS_S5 = csN[4];
    BE_CS_S4 = csN[5];
    BE_CS_S3 = csN[6];

    UARTWriteLine("3-8 Enable:");
    UARTWriteInt(csN[3],2);
    UARTWriteInt(csN[4],2);
    UARTWriteInt(csN[5],2);
    UARTWriteInt(csN[6],2);

    MCU_delay();

    BE_CS_S2 = csN[7];
    BE_CS_S1 = csN[8];
    BE_CS_S0 = csN[9];

    UARTWriteLine("3-8  Input:");
    UARTWriteInt(csN[7],2);
    UARTWriteInt(csN[8],2);
    UARTWriteInt(csN[9],2);

}

void CloseASIC() {
    BE_CS_S6 = 0;
    BE_CS_S5 = 0;
    BE_CS_S4 = 0;
    BE_CS_S3 = 0;
}

void ResetASIC(void) {
    SPI_Write(CMD_RESET);
    SetPll(CHIP_CLOCK-1);
}

void SendWork(u8 work[44]) {
    u8 i;
    for (i = 0; i < 44; i++) {
        SPI_Write(CMD_WRITE_REG(i));
        SPI_Write(work[i]);
    }
    SPI_Write(CMD_WRITE_REG(REG_START));
    SPI_Write(0);
}

void ReadWork(u8 work[44]) {
    u8 i;
    for (i = 0; i < 44; i++) {
        SPI_Write(CMD_READ_REG(i));
        work[i] = SPI_Read();
    }
}

u8 GetASIC(void) {
    SPI_Write(CMD_READ_REG(REG_STATUS));
    return (SPI_Read());
}


void IncNonce(u8 nonce[4]){
     u8 i;
     for(i=3;i<=0;i++){
         nonce[i]++;
         if(nonce[i]) break;
     }
}

u8 GetNonce(u8 *nonce, u8 clear) {
	u8 mask, addr;

	mask = GetASIC();
	if(!(mask & STAT_R_READY))
            return FALSE ;
	mask >>= 2;
        mask &= 0x0f;
	
	if(!mask)
            return FALSE ;
	if (mask & 0x01)
            addr = 46;
	else if (mask & 0x02)	
            addr = 50;
        else if (mask & 0x04)
            addr = 54;
        else
            addr = 58;

	SPI_Write(CMD_READ_REG(addr+3)); nonce[0] = SPI_Read();
	SPI_Write(CMD_READ_REG(addr+2)); nonce[1] = SPI_Read();
	SPI_Write(CMD_READ_REG(addr+1)); nonce[2] = SPI_Read();
	SPI_Write(CMD_READ_REG(addr  )); nonce[3] = SPI_Read();

        IncNonce(nonce);

	if (clear) {
		SPI_Write(CMD_READ_REG(REG_CLEAR));
                SPI_Read();
        }
	return TRUE;
}

