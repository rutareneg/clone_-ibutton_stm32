


#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>


    /*unsigned*/ char buf [17];
	float   d ;
	unsigned char list;//номера страници 1 уровня
//	unsigned char list_1;//номера страници 2 уровня

  _Bool nach;//контроль загрузки



//************настраиваем порты lcd***************//

#define lcd5110_DC       GPIO_Pin_9// D5 d/S   GPIO_Pin_10
#define lcd5110_RST      GPIO_Pin_8//  D4 reset GPIO_Pin_11
#define lcd5110_SCE	     GPIO_Pin_7//
#define lcd5110_MOSI     GPIO_Pin_15//  MOSI--D16
#define lcd5110_SCLK     GPIO_Pin_13// SCK--D15
#define lcd5110_LED      GPIO_Pin_12//  не используется
#define lcd5110_PORT     GPIOB

#define SPI_lcd  SPI2
#define RCC_GPIOB      	RCC_APB2Periph_GPIOB
#define RCC_SPI2     	RCC_APB1Periph_SPI2



#define	ill_on  GPIO_SetBits(PORT_ILL, PIN_ILL);
#define	ill_off  GPIO_ResetBits(PORT_ILL, PIN_ILL);

/************настраиваем порты кнопок***************/

#define	 RCC_PORT_KEY   RCC_APB2Periph_GPIOA
#define  PORT_KEY GPIOA
#define  PIN_KEY_1 GPIO_Pin_9
#define  PIN_KEY_2 GPIO_Pin_10
#define  PIN_KEY_3 GPIO_Pin_11
#define  PIN_KEY_4 GPIO_Pin_12

//*************порты нагрузки***************************//

#define  PORT_IN_OUT 		GPIOB  //порт управления нагрузкой
#define  PIN_OUT1 		GPIO_Pin_3 //выход управления нагрузкой1
#define  PIN_OUT2 		GPIO_Pin_4 //выход управления нагрузкой2
#define	 RCC_PORT_IN_OUT  RCC_APB2Periph_GPIOB

//******порт MCO****тактирование процессора для отладки******/
/*
#define	 RCC_PORT_MCO   RCC_APB2Periph_GPIOA
#define  PORT_MCO       GPIOA
#define  PIN_MCO        GPIO_Pin_8
*/

/******настройка порта для DS18B20**********************/
#define USART_DS18B20   USART3
#define Rx_USART_DS18B20 GPIO_Pin_11
#define Tx_USART_DS18B20 GPIO_Pin_10
#define PORT_USART_DS18B20 GPIOB
/*********************************************/

#define  RCC_PORT_ILL  RCC_APB2Periph_GPIOB
#define  PORT_ILL      GPIOB
#define  PIN_ILL       GPIO_Pin_1

#define  RCC_PORT_BIP  RCC_APB2Periph_GPIOA
#define  PORT_BIP      GPIOA
#define  PIN_BIP       GPIO_Pin_6

//#define  PORT_OUT GPIOC  //порт управления нагрузкой
//#define  PIN_OUT1 GPIO_Pin_14 //выход управления нагрузкой1
//#define  PIN_OUT2 GPIO_Pin_15 //выход управления нагрузкой2
#define  PIN_OUT1_OFF	GPIO_ResetBits(PORT_OUT,PIN_OUT1)
#define  PIN_OUT2_OFF	GPIO_ResetBits(PORT_OUT,PIN_OUT2)
#define  PIN_OUT2_ON	GPIO_SetBits(PORT_OUT,PIN_OUT2)
#define  PIN_OUT1_ON	GPIO_SetBits(PORT_OUT,PIN_OUT1)


#define  list_set_time    list == 1


#define  k_list key() == 2
#define  k_left key() == 1
#define  k_right key() == 3
#define	 save       k_left
#define	 repeat 	k_left
#define	 yes        keys==1
#define	 no         keys==3
#define  meny		keys==2
#define  meny_l		keys==20

#define  beck   0
//#define  meny		1
#define  setings	2
#define  ds_water	3
#define  ds_in	    4
#define  ds_out		5
#define	 ds_eng		6
		//позиции вывода информации

//"00-00 00:00:00"
//
#define	 pos_icon   2,0
#define  pos_bat	0,68


#define  pos_mday	0,0,8    //У,Х, размер шрифта
#define  pos_mon	0,18,8    //У,Х, размер шрифта
#define  pos_year   0,36,8    //У,Х, размер шрифта
#define  pos_dn  	0,66,8    //У,Х, размер шрифта

#define  pos_hour   1,2,24    //У,Х, размер шрифта
#define  pos_min    1,50,24    //У,Х, размер шрифта
#define  pos_sec    0,58,24    //У,Х, размер шрифта




#define  pos_lin     	5,45,8
#define  pos_in   		5,52,8
#define  pos_t_in   	5,76,8 //зак градусов

#define  pos_lout       4,45,8
#define  pos_out   		4,52,8
#define  pos_t_out  	4,76,8 //зак градусов

#define  pos_leng		4,0,8
#define  pos_eng   		4,7,8
#define  pos_t_eng		4,33,8 //зак градусов


#define  pos_nap    	5,7,8
#define  pos_v			5,33,8 //знак напряжения

//**************************************//

typedef struct
	{	_Bool ERROR;
		int   tmr_85;
		u8      code[8];//nюды номер ром код
		//u8      data[9];//всякие данные
		float   TCONTR;
		float   d;
		//uint32_t   adr;    //всего 64к пишем в 64 0x0800fC00 начальный адресс памяти для записи в память
	}  data_D18B20;

//*******************************************//
		data_D18B20 temp_out;// теумпер на улице
		data_D18B20 temp_water;//жидкость
		data_D18B20 temp_in;//в нутри
		data_D18B20 temp_eng;//двигатель

//*******************************************

//*******************************************//
		uint32_t   adr;
#define adr_start		    0x0800fC00

#define adr_temp_out  		0x0800fC00
#define adr_temp_water		0x0800fC14
#define adr_temp_in			0x0800fC28
//#define adr_temp_eng		0x0800fC3C
//*********************************************//
#define adr_control_out 	0x0800fC50
#define adr_control_water	0x0800fC64
#define adr_control_in		0x0800fC78
//#define adr_control_eng  	0x0800fC8C
//********************************************//
#define		adr_adc_calib   /*0x0800fCA0*/0x0800fC3C

		//*************************************//

		_Bool code_compare (unsigned char code[8], unsigned char compare_code[8]);
		void read_seatings();
		void code_set_ff ();
		void serch_d_ds18b20 (unsigned char serch_d[16]);
		void set_d_ds18b20( unsigned char set_d[16]);
		void set_control_ds18b20 (unsigned char set_d[16]);
		void save_ok ();
		void no_save ();
		void error_save();
		void convert_atl_ath ();
		//void Read_settings_to_flash ();
		//void Write_settings_to_flash ();
		void clock();
		void set_d_ds18b20 (unsigned char set_d[16]);
		void FLASH_read(uint16_t *p, u8 R);
		void read_seatings();
		void FLASH_Write (uint16_t *p, u8 R);
		void write_seatings();
		void set_def_list();
		void set_2t();//мигаем точками
		void messag (unsigned char mess[16]);
		void meny_driv();
		void Bat(u8 y,u8 x, u8 h,u8 c);
		void T_out(u8 y,u8 x, u8 h,u8 c);
		void T_in(u8 y,u8 x, u8 h,u8 c);
		void T_eng(u8 y,u8 x, u8 h,u8 c);

//********************************************//
		 void progress (u8 y,u8 p);
