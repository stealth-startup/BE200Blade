 

void delay(void)
{
 unsigned int i=60000;
 while(i--);
}


void MCU_init(void)
{
  
 TRISA=0X00;
 TRISB=0X00;
 TRISC=0x00;
 TRISD=0X00;
 TRISE=0X00;
 ADCON1=0X0F;
 
 PORTA=0XFF;
 PORTB=0X00;
  


}


















