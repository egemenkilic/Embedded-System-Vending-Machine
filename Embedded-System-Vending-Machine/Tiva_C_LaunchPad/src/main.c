#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>


#include <stdbool.h>
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#define BAUDRATE 600


#define RW 0x20 								// PA5
#define RS 0x40  								// PA6
#define E  0x80  								// PA7
//Aziz Yelbay 170201046  Egemen Kilic 170201084
volatile unsigned long delay;	// Port aktive ederken işimize yarayacak delay
volatile unsigned long gecikme;
int basmasayisi=0;
int secim=0;
char pu[7];
double para_ustu=0;
double asdas;
int led;
double sayim=0;
double paraye=0;
char *geciciptr;

int subas,kahvebas,caybas,cikobas,bisbas;
int birbas,ellibas,ybesbas;
int bir,elli,ybes;
int yenipara;

int kasa[3];
char dosya[6][30]={"20,20,10","1,su,30,50 kurus","2,cay,20,1 tl","3,kahve,15,1.5 tl","4,cikolata,50,1.75 tl","5,biskuvi,100,2 tl"};

struct myStruct
{
    int id;
    char ad[10];
    int stok_sayisi;


} urunler[5];
/** UART (seri port) ayarini yapan fonksiyon */
void init_UARTstdio() {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(0x00000001);
	GPIOPinConfigure(0x00000401);
	GPIOPinTypeUART(0x40004000, 0x00000001 | 0x00000002);
	UARTConfigSetExpClk(0x40004000, SysCtlClockGet(), BAUDRATE,
                        	(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         	UART_CONFIG_PAR_NONE));
	UARTStdioConfig(0, BAUDRATE, SysCtlClockGet());
}
void yerlestir(){
	for(int i=0; i<6; i++)
	    {
	        ////burasi ilk satirdaki kasadakileri para sayisi
	        if(i==0)
	        {
	            int kasacount=0;
	            char *ch;
	            ch = strtok(dosya[0], ",");
	            while (ch != NULL)
	            {

	                kasa[kasacount]=atoi(ch);
	                kasacount++;
	                ch = strtok(NULL, ",");
	            }
	            continue;

	        }
	        ////////

	        char dizi[5][15];
	        int sayac=0;
	        char *cha;
	        cha = strtok(dosya[i], ",");
	        while (cha != NULL)
	        {
	            sprintf(dizi[sayac],"%s", cha);

	            sayac++;
	            cha = strtok(NULL, ",");
	        }

	        urunler[i-1].id=atoi(dizi[0]);

	        geciciptr=dizi[1];
	        sprintf(urunler[i-1].ad,"%s", geciciptr);



	        urunler[i-1].stok_sayisi=atoi(dizi[2]);




	    }

}


// LCD komutları:
// http://michaelhuang69.blogspot.com.tr/2014/05/tm4c123gxl-launchpad-lcd1602.html

void portlariAktiflestir(void){
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;		// A portunu aktive et
	delay = SYSCTL_RCGC2_R;						// A portunun aktive edilmesini 1 tick bekle
	GPIO_PORTA_AMSEL_R &= ~0b11100000;			// A portunun analog modunu devre dışı bırak
	GPIO_PORTA_PCTL_R &= ~0xFFF00000;			// A portundaki pinlerin voltajını düzenle (PCTL=Power Control)
	GPIO_PORTA_DIR_R |= 0b11100000;				// A portunun giriş çıkışlarını belirle
	GPIO_PORTA_AFSEL_R &= ~0b11100000;			// A portundaki alternatif fonksiyonları seç
	GPIO_PORTA_DEN_R |= 0b11100000;				// A portunun pinlerini aktifleştir
	GPIO_PORTA_DR8R_R |= 0b11100000;			// A portundaki pinlerin 8mA çıkışını aktive et

	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;		// B portunu aktive et
	delay = SYSCTL_RCGC2_R;						// B portunun aktive edilmesini 1 tick bekle
	GPIO_PORTB_AMSEL_R &= ~0b11111111;			// B portunun analog modunu devre dışı bırak
	GPIO_PORTB_PCTL_R &= ~0xFFFFFFFF;			// B portundaki pinlerin voltajını düzenle (PCTL=Power Control)
	GPIO_PORTB_DIR_R |= 0b11111111;				// B portunun giriş çıkışlarını belirle
	GPIO_PORTB_AFSEL_R &= ~0b11111111;			// B portundaki alternatif fonksiyonları seç
	GPIO_PORTB_DEN_R |= 0b11111111;				// B portunun pinlerini aktifleştir
	GPIO_PORTB_DR8R_R |= 0b11111111;			// B portundaki pinlerin 8mA çıkışını aktive et

	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;		// E portunu aktive et
	delay = SYSCTL_RCGC2_R;						// E portunun aktive edilmesini 1 tick bekle
	GPIO_PORTE_DIR_R |= 0x00;					// E portunun giriş çıkışlarını belirle
	GPIO_PORTE_DEN_R |= 0b00011111;				// E portunun pinlerini aktifleştir

}


void komutGonder(unsigned char LCD_Comment){
	GPIO_PORTA_DATA_R &= ~(RS+RW+E);			// Tüm pinleri sıfırla
	GPIO_PORTB_DATA_R = LCD_Comment;			// Komutu yazdır
	GPIO_PORTA_DATA_R |= E;						// E'yi aç
	GPIO_PORTA_DATA_R &= ~(RS+RW);				// RS ve RW kapat
	for (delay = 0 ; delay < 1; delay++);		// 1us bekle
	GPIO_PORTA_DATA_R &= ~(RS+RW+E);			// RS, RW ve E kapat
	for (delay = 0 ; delay < 1000; delay++);	// 1ms bekle
}

void veriGonder(unsigned char LCD_Data){
	GPIO_PORTB_DATA_R = LCD_Data;				// Write Data
	GPIO_PORTA_DATA_R |= RS+E;					// RS ve E aç
	GPIO_PORTA_DATA_R &= ~RW;					// RW kapat
	for (delay = 0 ; delay < 23 ; delay++);		// 230ns bekle
	GPIO_PORTA_DATA_R &= ~(RS+RW+E);			// RS, RW ve E kapat
	for (delay = 0 ; delay < 1000; delay++);	// 1ms bekle
}

void ekraniAktiflestir(){
	portlariAktiflestir();						// Portları aktifleştir
	for (delay = 0 ; delay < 15000; delay++);	// 15ms bekle
	komutGonder(0x38);							// 0b00111000 -> PortB
	for (delay = 0 ; delay < 5000; delay++);	// 5ms bekle
	komutGonder(0x38);							// 0b00111000 -> PortB
	for (delay = 0 ; delay < 150; delay++);		// 150us bekle
	komutGonder(0x0C);							// 0b00001010 -> PortB
	komutGonder(0x01);							// Ekranı Temizle
	komutGonder(0x06);							// 0b00000110 -> PortB
	for (delay = 0 ; delay < 50000; delay++);	// 50ms bekle
}

void ekranaYazdir(unsigned int line,unsigned int digit, unsigned char *str){
	unsigned int lineCode = line==1 ?0x80:0xC0;	// Line 1 ise 0x80, 2 ise 0xC0 komutu kullanılması gerekiyor
	komutGonder(lineCode + digit);				// Yazının nereden başlayacağını LCD'ye bildir
	while(*str != 0){ veriGonder(*str++); }		// Ve yazdır
}







int basiliButon(){								// Şu anda basılı olan butonu döndüren fonksiyon
	return  (GPIO_PORTE_DATA_R & 0b00010000) == 0 ? 1
			: (GPIO_PORTE_DATA_R & 0b00100000) == 0 ? 2
					: (GPIO_PORTE_DATA_R & 0b00001000) == 0 ? 3
							: (GPIO_PORTE_DATA_R & 0b00000100) == 0 ? 4
									: (GPIO_PORTE_DATA_R & 0b00000010) == 0 ? 5
											: (GPIO_PORTE_DATA_R & 0b00000001) == 0 ? 6
													: (GPIO_PORTF_DATA_R & 0b00001) == 0 ? 7
															: (GPIO_PORTF_DATA_R & 0b10000) == 0 ? 8
																	: (GPIO_PORTF_DATA_R & 0b00010) == 0 ? 9
			: 0 ;
}




void init_port_E(){

	volatile unsigned long tmp; // bu degisken gecikme yapmak icin gerekli
	   SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;   // 1) E portunun osilatörünü etkinleştir
	   tmp = SYSCTL_RCGCGPIO_R;    	// allow time for clock to start
	   GPIO_PORTE_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port E
	   GPIO_PORTE_CR_R = 0x3F;         // allow changes to PE5-0 //PE5-0 değişikliklerine izin ver
	                                   // only PE0 needs to be unlocked, other bits can't be locked
	    			 // Sadece PE0 kilidinin açılması gerekir, diğer bitler kilitlenemez
	   GPIO_PORTE_AMSEL_R = 0x00;    	// 3) disable analog on PE //PE'de analog devre dışı bırak
	   GPIO_PORTE_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PE4-0
	   //eski GPIO_PORTE_DIR_R = 0x0F;      	// 5) PE4,PE5 in, PE3-0 out
	   //GPIO_PORTE_DIR_R = 0x07;// 5) PE3,PE4,PE5 in, PE2-0 out
	   //GPIO_PORTE_DIR_R = 0x03;//PE2,PE3,PE4,PE5 in, PE1-0 out
	   //GPIO_PORTE_DIR_R = 0x01;
	   GPIO_PORTE_DIR_R = 0x00;
	   GPIO_PORTE_AFSEL_R = 0x00;    	// 6) disable alt funct on PE7-0

	   //eski GPIO_PORTE_PUR_R = 0x30; //PE4 ve PE5'te pull up'ı etkinleştir ( BUTON İÇİN)// enable pull-up on PE5 and PE4
	   //GPIO_PORTE_PUR_R = 0x38;      	//PE3,PE4 ve PE5'te pull up'ı etkinleştir ( BUTON İÇİN)//
	   //GPIO_PORTE_PUR_R = 0x3c;         //PE2,PE3,PE4 ve PE5'te pull up'ı etkinleştir ( BUTON İÇİN)//
	   //GPIO_PORTE_PUR_R = 0x3E;
	   GPIO_PORTE_PUR_R = 0x3F;
	   GPIO_PORTE_DEN_R = 0x3F;      	// 7) enable digital I/O on PE5-0 // portE 5-0 giriş çıkış  etkinlerştir.

}



void init_portx_F() {
   volatile unsigned long tmp; // bu degisken gecikme yapmak icin gerekli
   tmp = SYSCTL_RCGCGPIO_R;    	// allow time for clock to start
   SYSCTL_RCGCGPIO_R |= 0x00000020;  // 1) activate clock for Port F
   GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
   GPIO_PORTF_CR_R = 0x01;       	// allow changes to PF-0
   // only PF0 needs to be unlocked, other bits can't be locked
   //eskiGPIO_PORTF_DIR_R = 0x0E;// 5) PF4,PF0 in, PF3-1 out
   GPIO_PORTF_DIR_R = 0xC;//yeni
   //eskiGPIO_PORTF_PUR_R = 0x11;  // enable pull-up on PF0 and PF4
   GPIO_PORTF_PUR_R = 0x13;//yeni
   GPIO_PORTF_DEN_R = 0x1F;      	// 7) enable digital I/O on PF4-0
}







int main(void) {

	init_port_E();
	ekraniAktiflestir();
	init_portx_F();
yerlestir();

init_UARTstdio();

subas=0;
kahvebas=0;
cikobas=0;
caybas=0;
bisbas=0;

birbas=0;
ellibas=0;
ybesbas=0;
basmasayisi=0;

bir=0;
elli=0;
ybes=0;





int ikinciasama=0;
ikinciasama=0;
					GPIO_PORTF_DATA_R &= ~(0b00100);
					GPIO_PORTF_DATA_R &= ~(0b01000);
					sayim=0;
					paraye=0;
					para_ustu=0;

while(1){
		if(basiliButon()){										// Eğer herhangi bir butona basılmışsa
			secim = basiliButon();								// Hangi butona basıldığını bir değişkene ata
		}else{													// Buton bırakıldığında (veya hiç basılmadığında)
			if(secim!=0){										// Herhangi bir buton bırakıldıysa
					 if(secim==1){
						 if(ikinciasama==0){
							 komutGonder(0x01);
							 	 	 	 	 	 basmasayisi++;
						 						 sayim=sayim+0.25;
						 						 double virgulsayim;
						 						 virgulsayim=sayim*100;
						 						 virgulsayim=(int)virgulsayim%100;
						 						 char fullStr[16] = {((int)sayim)+48,'.',(int)virgulsayim/10+48,(int)virgulsayim%10+48,' ', 't','l',' ','a','t','t','i','n','i','z','\0'};
						 						 ekranaYazdir(1,0,fullStr);
						 						 kasa[0]++;
						 						 ybesbas++;

						 }
						 if(ikinciasama==1){
							 komutGonder(0x01);
							 basmasayisi++;
							 paraye=paraye+0.50;
							 double fiyat;
							 fiyat=paraye*100;
							 fiyat=(int)fiyat%100;
							 char yazi[7]={((int)paraye)+48,'.',(int)fiyat/10+48 ,(int)fiyat%10+48 ,'t','l','\0' };
							 ekranaYazdir(1,0,yazi);
							 ekranaYazdir(2,0,"su aldiniz");
							 urunler[0].stok_sayisi--;
							 subas++;
						 }







					 }
				else if(secim==2){
					if(ikinciasama==0){

						komutGonder(0x01);
						basmasayisi++;
										sayim=sayim+0.50;
										double virgulsayim;

										    virgulsayim=sayim*100;
										    virgulsayim=(int)virgulsayim%100;
										    char fullStr[16] = {((int)sayim)+48,'.',(int)virgulsayim/10+48,(int)virgulsayim%10+48,' ', 't','l',' ','a','t','t','i','n','i','z','\0'};
										    ekranaYazdir(1,0,fullStr);
										    kasa[1]++;
										    ellibas++;
					}
					if(ikinciasama==1){
						 komutGonder(0x01);
						 basmasayisi++;
													 paraye=paraye+1;
													 double fiyat;
													 fiyat=paraye*100;
													 fiyat=(int)fiyat%100;
													 char yazi[7]={(int)paraye+48,'.',(int)fiyat/10+48 ,(int)fiyat%10+48 ,'t','l','\0' };
													 ekranaYazdir(1,0,yazi);
													 ekranaYazdir(2,0,"cay aldiniz");
													 urunler[1].stok_sayisi--;
													 caybas++;
					}


				}

				else if(secim==3){
					if(ikinciasama==0){
						komutGonder(0x01);
						basmasayisi++;
											sayim++;
											double virgulsayim;
											virgulsayim=sayim*100;
											virgulsayim=(int)virgulsayim%100;

											char fullStr[16] = {((int)sayim)+48,'.',(int)virgulsayim/10+48,(int)virgulsayim%10+48,' ', 't','l',' ','a','t','t','i','n','i','z','\0'};
											ekranaYazdir(1,0,fullStr);
											kasa[2]++;
											birbas++;
					}
					if(ikinciasama==1){
						 komutGonder(0x01);
						 basmasayisi++;
													 paraye=paraye+1.50;
													 double fiyat;
													 fiyat=paraye*100;
													 fiyat=(int)fiyat%100;
													 char yazi[7]={(int)paraye+48,'.',(int)fiyat/10+48 ,(int)fiyat%10+48 ,'t','l','\0' };
													 ekranaYazdir(1,0,yazi);
													 ekranaYazdir(2,0,"kahve aldiniz");
													 urunler[2].stok_sayisi--;
													 kahvebas++;
					}

								}
				else if(secim==4){
					if(ikinciasama==1){
											 komutGonder(0x01);
											 basmasayisi++;
																		 paraye=paraye+1.75;
																		 double fiyat;
																		 fiyat=paraye*100;
																		 fiyat=(int)fiyat%100;
																		 char yazi[7]={(int)paraye+48,'.',(int)fiyat/10+48 ,(int)fiyat%10+48 ,'t','l','\0' };
																		 ekranaYazdir(1,0,yazi);
																		 ekranaYazdir(2,0,"cikolata aldiniz");
																		 urunler[3].stok_sayisi--;
																		 cikobas++;
										}


												}
				else if(secim==5){
					if(ikinciasama==1){
					 komutGonder(0x01);
					 basmasayisi++;
												 paraye=paraye+2;
												 double fiyat;
												 fiyat=paraye*100;
												 fiyat=(int)fiyat%100;
												 char yazi[7]={(int)paraye+48,'.',(int)fiyat/10+48 ,(int)fiyat%10+48 ,'t','l','\0' };
												 ekranaYazdir(1,0,yazi);
												 ekranaYazdir(2,0,"biskuvi aldiniz");
												 urunler[4].stok_sayisi--;
												 bisbas++;

				}


																}
				else if(secim==6){
					//asama degistirme butonu
					komutGonder(0x01);
					ekranaYazdir(1,0,"urun secim islemi");
					ikinciasama=1;

				}
				else if(secim==7){
					//para takilmasi oldugunda kirmizi led yanar.
					komutGonder(0x01);
					if((basmasayisi%4)+1==2){
						ekranaYazdir(1,0,"para takildi");
						GPIO_PORTF_DATA_R |= 0b01000;







					}else{
						para_ustu=sayim-paraye;

											asdas=para_ustu*100;
											asdas=(int)asdas%100;
											pu[0]=(int)para_ustu+48;
											pu[1]='.';
											pu[2]=(int)asdas/10+48;
											pu[3]=(int)asdas%10+48 ;
											pu[4]='t' ;
											pu[5]='l' ;
											pu[6]='\0';
											ekranaYazdir(1,0,pu);
											ekranaYazdir(2,0,"Para ustunuz");
											GPIO_PORTF_DATA_R |= 0b00100;
											//GPIO_PORTF_DATA_R &= ~(0b01000);
											//yesil yak

										    yenipara=para_ustu*100;
										    bir=yenipara/100;
										    yenipara=yenipara-(bir*100);

										    elli=yenipara/50;
										    yenipara=yenipara-(elli*50);

										    ybes=yenipara/25;
										    yenipara=yenipara-(ybes*25);

										    kasa[0]=kasa[0]-ybes;
										    kasa[1]=kasa[1]-elli;
										    kasa[2]=kasa[2]-bir;


										    bir=0;
										    elli=0;
										    ybes=0;




											for (int i = 0 ;i<8 ; i++) {

											   	if(i>4){
											   		UARTprintf("%d\n",kasa[7-i]);

											   	}
											   	else
											   		UARTprintf("Stok %s= %d\n", urunler[i].ad,urunler[i].stok_sayisi);


												}





					}






				}
				else if(secim==8){
					komutGonder(0x01);
					ikinciasama=0;
					GPIO_PORTF_DATA_R &= ~(0b00100);
					GPIO_PORTF_DATA_R &= ~(0b01000);
					sayim=0;
					paraye=0;
					para_ustu=0;



					urunler[0].stok_sayisi=urunler[0].stok_sayisi + subas;
											urunler[1].stok_sayisi=urunler[1].stok_sayisi + caybas;
											urunler[2].stok_sayisi=urunler[2].stok_sayisi + kahvebas;
											urunler[3].stok_sayisi=urunler[3].stok_sayisi + cikobas;
											urunler[4].stok_sayisi=urunler[4].stok_sayisi + bisbas;

											kasa[0]=kasa[0]-ybesbas;
											kasa[1]=kasa[1]-ellibas;
											kasa[2]=kasa[2]-birbas;

											subas=0;
											kahvebas=0;
											cikobas=0;
											caybas=0;
											bisbas=0;

											birbas=0;
											ellibas=0;
											ybesbas=0;
											basmasayisi=0;

											bir=0;
											elli=0;
											ybes=0;

					ekranaYazdir(1,0,"resetlendi");
				}
				else if(secim==9){
					//basa donme
					komutGonder(0x01);
					ikinciasama=0;
					sayim=0;
					paraye=0;
					para_ustu=0;


					subas=0;
					kahvebas=0;
					cikobas=0;
					caybas=0;
					bisbas=0;

					birbas=0;
					ellibas=0;
					ybesbas=0;
					basmasayisi=0;

					bir=0;
					elli=0;
					ybes=0;
					GPIO_PORTF_DATA_R &= ~(0b00100);
					ekranaYazdir(1,0,"Basa Donme");
				}




			}
			secim=0;											// Seçimi sıfırla
		}

	    for (delay = 0 ; delay < 1000 ; delay++);
	}

	}













