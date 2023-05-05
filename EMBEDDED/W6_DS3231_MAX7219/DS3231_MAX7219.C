//gcc -o bt1 DS3231_MAX7219.c -lwiringPi
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringPiSPI.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>

#define channel 0

void send_data(uint8_t address, uint8_t value)
{
    uint8_t data[2];
    data[0]=address;
    data[1]=value;
    wiringPiSPIDataRW(channel, data, 2);
    //return 0; //gui tra du lieu tu ic ve (neu ic co chuc nang gui tra du lieu)
}

void Initmax7219()
{
    //decode mode: 0x09FF
    send_data(0x09, 0xFF);  //chon mode dung so thap phan
    //intensity: 0x0A08
    send_data(0x0A,0x08); // chon cuong do sang cua led
    //scanlimit:
    send_data(0x0B,7); // chon so luong digit (so led)
    //no shutdown, display test off
    send_data(0x0C,1);
    send_data(0x0F,0);
}
uint8_t dec2hex (uint8_t d)
{
  uint8_t h;
  h =(d/10 <<4)|(d%10 );
    return h;

}


int main(void)
{
    //setup giao tiep I2C 
    int ds = wiringPiI2CSetup(0x68);
    // read raspberry time
    time_t T = time(NULL);
    struct tm tm = *localtime(&T);
    printf("system date is: %02d/%02d/%04d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    printf("system time is: %02d:%02d:%02d\n", tm.tm_hour, tm.tm_min , tm.tm_sec);


    // chuyen sang hex, 
  
    wiringPiI2CWriteReg8(ds,0x00,dec2hex(tm.tm_sec));
    wiringPiI2CWriteReg8(ds, 0x01,dec2hex(tm.tm_min));
    wiringPiI2CWriteReg8(ds,0x02,dec2hex(tm.tm_hour));
    // check ds3231 time

    unsigned int hh, mm, ss; 
    unsigned int chuc_gio,dv_gio,chuc_phut,dv_phut,chuc_giay,dv_giay;

    ss = wiringPiI2CReadReg8(ds, 0x00);
    mm = wiringPiI2CReadReg8(ds, 0x01);
    hh = wiringPiI2CReadReg8(ds, 0x02);

    //setup SPI interface
    wiringPiSPISetup(channel, 8000000);  

    Initmax7219();

    //
    for(;hh<0x24;)
    {
      for(;mm<0x60;)
      {
        if(mm==0x60)
        {
          mm=0x00;
        }
        for(;ss<0x60;)
        {        
          //GIAY
            // VD neu ss = 0x39 (39s) => khi ++ se thanh 0x3a => Vi vay ss =ss + 0x05 = 0x3f => ss = 0x3f + 1 = 0x40
            if(ss == 0x0a || ss == 0x1a || ss == 0x2a || ss ==0x3a || ss==0x4a)
            {
              ss = ss | 0x05;
              ss++;
            }
            // Neu ss = 0x59 => khi ++ se thanh 0x5a => Gan ss = 0x60 luon.
            if(ss==0x5a)
            {
              ss = 0x60;
              break;
            }

          //PHUT
            
            if(mm == 0x0a || mm == 0x1a || mm == 0x2a || mm ==0x3a || mm ==0x4a)
            {
              mm = mm | 0x05;
              mm++;
            }
            if(mm==0x5a)
            {
              mm = 0x60;
              break;
            }

          //GIO
            
            if(hh == 0x0a || hh == 0x1a)
            {
              hh = hh | 0x05;
              hh++;
            }
          
          chuc_gio = hh>>4 | 0x00;
          dv_gio = hh & 0x0f;
          chuc_phut = mm>>4 | 0x00;
          dv_phut = mm & 0x0f;
          chuc_giay = ss>>4 | 0x00;
          dv_giay = ss & 0x0f;
          uint8_t ht[8] = {chuc_gio,dv_gio,0x0a,chuc_phut,dv_phut,0x0a,chuc_giay,dv_giay};
          for(int i=0; i<8;i++)
            {
                send_data(i+1, ht[7-i]);
            }

          printf("Gia tri cua ds3231 : %x:%x:%x \n",hh,mm,ss);
          
          delay(1000);  
          ss++;       
        }
        if(ss==0x60)
        {
          ss = 0x00;
        }
        mm++;
      }
      if(mm==0x60)
      {
        mm = 0x00;
      }
      hh++;
      if(hh==0x24)
      {
        hh=0x00;
      }
    }      
    return 0;
}