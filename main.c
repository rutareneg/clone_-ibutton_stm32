

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

    	   while(code[0]==0)
    	   {//выполняем пока не найден ключ
    	      	   	   lcd_set_rect(3,0,84*2,1);
    	      	   	   if(Reset_USART_DS18B20())
    	         		{//забираем память
    	         		Get_Rom_USART_DS18B20(code);

    	         		SRC = chek_crc(code,7);
    	         		if(code[7]!=SRC){lcd_set_strs(3,0,16,"   ERROR  ",0); BIP(150,300);delay_ms(1000);code[0]=0x00; break;}
    	         		    	         		    	         		//_sprtffd(0,buf, code[7]);
    	         		for(u8 i=0;i<8;i++){_sprtf16(data,code[i]); buf[i*2]= data[0];
    	         						buf[i*2+1]= data[1];
    	         						buf[i*2+2]= 0;}

    	         		lcd_set_strs(3,0,8,buf,0);

    	         		}
    	         		else {lcd_set_strs(3,0,8,"КЛЮЧ НЕ НАЙДЕН ",0);
    	         		delay_ms(400); }


    	      	   	switch ( code[0] ) {
    	      	   	case 0x01:     lcd_set_strs(0,0,8,"1990,2401,2411",0); break; //собщение
    	      	   	case 0x02:     lcd_set_strs(0,0,8,"     DS1991    ",0); break;//собщение
    	      	   	case 0x04:	   lcd_set_strs(0,0,8," DS1994 DS2404  ",0); break;
    	      	   	default:  lcd_set_strs(3,0,8,"КЛЮЧ НЕ НАЙДЕН ",0);	break;
    	      	   						}
    	      	  	  	lcd_set_strs(2,3,8,"*****ID******",0);//
    	      	  	 if(code[0]>0x00){lcd_set_strs(5,18,8,"ЗАПИСАТЬ",0); BIP(150,300);delay_ms(400);}

    	   }


    	         	if(keys==2)	{L=1;}

    	         	while(L && code[0]==0x01)
    	         	{key_st(10);lcd_set_strs(5,18,8,"ОТМЕНИТЬ",0);



    	         		if(Reset_USART_DS18B20())
    	         		{	message("*              ", 0);
    	         			Get_Rom_USART_DS18B20(code1);
    	         			if(!chek_code(code, code1)){message("КОД СУЩЕСТВУЕТ", 0); delay_s(3); break;}


    	         	if(Reset_USART_DS18B20())
    	         	{  message("***            ", 0);
    	         		Write_USART_DS18B20(0xD1); //разрешить запись
    	         		Write_bit_USART_DS18B20(0);
    	         		 delay_ms(16);}
    	         	else {message("    ERRORS 2   ", 0); break;}

    	         	if(Reset_USART_DS18B20())
    	       	{   message("*****          ", 0);
    	         		Write_USART_DS18B20(0x1E); //разрешить запись
    	         	  d =	Read_bits_USART_DS18B20(1);
    	         _sprtf16(buf,d);lcd_set_strs(3,0,8,buf,0);delay_ms(15);}
    	         	else {message("    ERRORS 3   ", 0); break;}

    	         	if(Reset_USART_DS18B20())
    	         			{message("*******        ", 0);
    	         		Write_USART_DS18B20(0xD5);
    	         			prog_USART_byts(code);
    	         			delay_ms(10);}
    	         	else {message("    ERRORS 4   ", 0); break;}

    	         		if(Reset_USART_DS18B20())
    	         			{message("*********      ", 0);

    	         			Write_USART_DS18B20(0xD1);
    	         			Write_bit_USART_DS18B20(1);
    	         			delay_ms(200);}
    	         	else {message("    ERRORS 5   ", 0); break;}

    	         			Get_Rom_USART_DS18B20(code1);
    	         			message("***********    ", 0);
    	         		for(u8 i=0;i<8;i++){_sprtf16(data,code1[i]); buf[i*2]= data[0];
    	         		    	         						buf[i*2+1]= data[1];
    	         		    	         						buf[i*2+2]= 0;}

    	         		    	         		lcd_set_strs(3,0,8,buf,0);


    	         			if(chek_code(code, code1)){message(" ОШИБКА ЗАПИСИ", 0);delay_s(3); break;}
    	         			else {message("**************", 0);delay_ms(500);

    	         				message("     ГОТОВ     ",0); delay_s(1);
    	         			message(" УБЕРИТЕ КЛЮЧ ",0);delay_s(5); break;}

    	         			//if(SRC = chek_crc(code,7)){}
    	         			//else {lcd_set_strs(3,5,8,"ОШИБКА ЗАПИСИ",0); delay_ms(500);L==0;}

    	         		}else {lcd_set_rect(3,0,84*2,1);
    	         			lcd_set_strs(3,3,8,"ВСТАВЬТЕ КЛЮЧ",0);  delay_ms(1000);L==0;}
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
	 lcd_set_strs(3,0,8,buf,i); BIP(150,300); delay_ms(1000);L==0;
 }

