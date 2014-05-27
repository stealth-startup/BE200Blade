#ifndef BE200_H
#define	BE200_H

#include "include.h"

#define ASICS 32
#define TRUE 1
#define FALSE 0

#define REG_START  44
#define REG_PLL    45
#define REG_CLEAR  62
#define REG_STATUS 63

#define CMD_CHECK              0x00
#define CMD_RESET              0xc0
#define CMD_WRITE_REG(n)       (0x80 | (n))
#define CMD_READ_REG(n)        (0x40 | (n))

#define STAT_W_ALLOW           0x01
#define STAT_R_READY           0x02

extern u8 CHIP_CLOCK;

void SetPll(u8 val);
void Set_BS(void);
void Clr_BS(void);
void OpenASIC(u8 n);
void CloseASIC();
void ResetASIC(void);
void SendWork(u8 work[44]);
void ReadWork(u8 work[44]);
u8 GetASIC(void);
u8 GetNonce(u8 *nonce, u8 clear);

#endif	

