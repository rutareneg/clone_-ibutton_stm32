

#include "main.h"

#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <core_cm3.h>
#include "stm32f10x_conf.h"





#include "delay.h"


#include "ds18b20.h"
#include "key.h"
#include "nokia_5110.h"
#include "prtf.h"


unsigned char code1[8];//сюды номер ром код
u8 chek_code(u8 *dat1, u8 *dat2);
void  message(unsigned char *buf, u8 i);
 u8 L=0;
 u8 SRC=0; //контрольная сумма


int main(void)
{


	delay_init(72);//уменьшаем скорость для экономии энерги 8м

	key_ini();// настройка портов кнопок и зв сиг
	gpio_spi_Init();
	init_lcd_5110 ();

	blinc_init();
	Init_USART_GPIO_DS18B20();
		ILL_init();

	 	ILL_ON();
	 	lcd_set_rect(1,0,84*4,0);
	 	lcd_set_strs(2,6,16,"ROTARENEG",1);
		delay_s(1);
		ILL_OFF();
	 	lcd_clear();



//*******************************************************************************************************************************************//
    while(1)
    {  IWDG_ReloadCounter();//перезапуск вачдога

    	   key_st(10);

    	  	//IWDG_ReloadCounter();

    	   while(keys!=2)
    	   {


    	      	   	   lcd_set_strs(1,3,8,"*****ID******",0);//
    	      	   	   if(Reset_USART_DS18B20())
    	         		{//забираем память
    	         		Get_Rom_USART_DS18B20(code);

    	         		SRC = chek_crc(code,7);
    	         		if(code[7]!=SRC){lcd_set_strs(3,0,16," ERROR_CRC ",0); BIP(150,300);delay_s(1); code[0]=0;break;}

    	         		for(u8 i=0;i<7;i++){_sprtf16(data,code[i]); buf[i*2]= data[0];
    	         						buf[i*2+1]= data[1];
    	         						buf[i*2+2]= 0;}

    	         		lcd_set_strs(2,0,8,buf,0);

    	         		_sprtf16(buf,code[7]);// пишем отдельно контрольную сумму
    	         		lcd_set_strs(3,0,8,"   SRS <  >    ",0);
    	         		lcd_set_strs(3,48,8,buf,0);
    	         		BIP(150,300);
    	         		}




    	      	   	switch ( code[0] ) {
    	      	   	case 0x01:     lcd_set_strs(0,0,8,"1990,2401,2411",0); break; //собщение
    	      	   	case 0x02:     lcd_set_strs(0,0,8,"     DS1991    ",0); break;//собщение
    	      	   	case 0x04:	   lcd_set_strs(0,0,8," DS1994 DS2404  ",0); break;
    	      	   	default:  	   lcd_set_rect(4,0,84,1); lcd_set_strs(3,0,8,"КЛЮЧ НЕ НАЙДЕН",0);	break;

    	      	   						}

    	      	  	 if(code[0]>0x00){lcd_set_strs(5,18,8,"ЗАПИСАТЬ",0); delay_ms(400);
    	      	  	 }

    	      	  	 key_st(10);
    	   }

    	         	if(keys==2)	{L=1;}

    	         	while(L && code[0]==0x01)
    	         	{
    	         		key_st(10); lcd_set_strs(5,18,8,"ОТМЕНИТЬ",0);

    	         		if(Reset_USART_DS18B20())
    	         		{
    	         			lcd_set_rect(3,0,84*2,1);
    	         			progress (3,10);
    	         			Get_Rom_USART_DS18B20(code1);
    	         			if(!chek_code(code, code1)){message("КОД СУЩЕСТВУЕТ", 0); delay_s(1);}
    	         			else
    	         			{

    	         	if(Reset_USART_DS18B20())
    	         	{
    	         		progress (3,20);
    	         		Write_USART_DS18B20(0xD1); //разрешить запись
    	         		Write_bit_USART_DS18B20(0);
    	         		 delay_ms(15);}
    	         	else {message("    ERRORS 2   ", 0); break;}

    	         	if(Reset_USART_DS18B20())
    	       	{
    	         		progress (3,30);
    	         		Write_USART_DS18B20(0x1E);
    	         	  d =	Read_bits_USART_DS18B20(1);
    	         	 delay_ms(15);}
    	         	else {message("    ERRORS 3   ", 0); break;}

    	         	if(Reset_USART_DS18B20())
    	         			{
    	         		progress (3,40);
    	         		Write_USART_DS18B20(0xD5);
    	         			prog_USART_byts(code);
    	         			delay_ms(10);}
    	         	else {message("    ERRORS 4   ", 0); break;}

    	         		if(Reset_USART_DS18B20())
    	         			{
    	         			progress (3,50);

    	         			Write_USART_DS18B20(0xD1);
    	         			Write_bit_USART_DS18B20(1);
    	         			delay_ms(200);}
    	         	else {message("    ERRORS 5   ", 0); break;}

    	         			Get_Rom_USART_DS18B20(code1);

    	         			progress (3,61);

    	         			if(chek_code(code, code1)){message("ОШИБКА ЗАПИСИ ", 0);delay_s(2);}

    	         			else {

    	         				progress (3,72);

    	         				message("     ГОТОВ    ",0); delay_s(1);
    	         			message(" УБЕРИТЕ КЛЮЧ ",0);delay_s(2); }

    	         			}
    	         		}
    	         		else {lcd_set_rect(3,0,84*2,1);
    	         			lcd_set_strs(3,3,8,"ВСТАВЬТЕ КЛЮЧ",0); delay_s(1);}

    	         		if(keys==2)	{L=0;code[0]=0; lcd_clear();}
    	         	}

    	       }
    }




//*******************************************************************************************************************************************//

 u8 chek_code(u8 *dat1, u8 *dat2)
 {
	 for (u8 i = 0; i < 8; i++)
	 						{
	 							if(*dat1++ != *dat2++) return 1;

	 						}return 0;
 }

 void  message(unsigned char *buf, u8 i)//i нвертирование
 {
	 lcd_set_rect(3,0,84*2,1);
	 lcd_set_strs(3,0,8,buf,i); BIP(150,300); delay_s(1);L==0;
 }

 void progress (u8 y,u8 p)//р прогресс 0-66

 {
	 lcd_set_strs(y,0,8,"[",0);
	 lcd_set_strs(y,78,8,"]",0);

	 if(p>72){p=72;}

	 lcd_set_rect(y,6,p,0);

	 BIP(150,300);delay_ms(300);

 }

