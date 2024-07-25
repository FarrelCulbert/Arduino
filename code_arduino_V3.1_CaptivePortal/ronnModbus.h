/*
 * GET DATA FROM PM5350 (MODBUS RTU)
 * ***********************************************************************************
 * Code by : fahroni|ganteng
 * contact me : fahroniganteng@gmail.com
 * Date : feb 2021
 * License :  MIT
 * 
 */


#include "Arduino.h"
#include "SimpleModbusMaster.h"


// NOTE 
// `````````````````````````````````````````````````````````````````````````````````````
// To change serial config (data bit, parity, & stop bit) go to "SimpleModbusMaster.cpp" on line 355 
// arduino default config : SERIAL_8N1
// PM5350 default config : SERIAL_8E1
// ref : https://www.arduino.cc/reference/en/language/functions/communication/serial/begin/

#define baud 19200
#define timeoutt 100000
#define polling 250 // the scan rate


#define retry_count 10 
#define TxEnablePin 0 // pin 0 & pin 1 are reserved for RX/TX. To disable set _TxEnablePin < 2 

unsigned int connection_status;



// PACKET MAPPING
// ``````````````````````````````
// Check on register list of PM5350 :
// https://www.se.com/id/en/download/document/Public+PM5350+v1.11.0+Register+List/
unsigned int data1[12]; // current       ==> 2999 - 3010
unsigned int data2[18]; // voltage       ==> 3019 - 3036
unsigned int data3[24]; // power         ==> 3053 - 3076
unsigned int data4[16]; // Power Factor  ==> 3077 - 3092
unsigned int data5[2];  // frequancy     ==> 3109 - 3110
unsigned int data6[4];  // date time     ==> 1836 - 1841 3203 - 3206

enum{
  PACKET1,
  PACKET2,
  PACKET3,
  PACKET4,
  PACKET5,
  PACKET6,
  TOTAL_NO_OF_PACKETS // leave this last entry
};

Packet packets[TOTAL_NO_OF_PACKETS];
packetPointer packet1 = &packets[PACKET1];
packetPointer packet2 = &packets[PACKET2];
packetPointer packet3 = &packets[PACKET3];
packetPointer packet4 = &packets[PACKET4];
packetPointer packet5 = &packets[PACKET5];
packetPointer packet6 = &packets[PACKET6];



// MODBUS INIT
// `````````````````````````````````````````
void modbusInit(){
  packet1->id = 1;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = 2999;
  packet1->no_of_registers = ARRSIZE(data1);
  packet1->register_array = data1;

  packet2->id = 1;
  packet2->function = READ_HOLDING_REGISTERS;
  packet2->address = 3019;
  packet2->no_of_registers = ARRSIZE(data2);
  packet2->register_array = data2;

  packet3->id = 1;
  packet3->function = READ_HOLDING_REGISTERS;
  packet3->address = 3053;
  packet3->no_of_registers = ARRSIZE(data3);
  packet3->register_array = data3;

  packet4->id = 1;
  packet4->function = READ_HOLDING_REGISTERS;
  packet4->address = 3077;
  packet4->no_of_registers = ARRSIZE(data4);
  packet4->register_array = data4;

  packet5->id = 1;
  packet5->function = READ_HOLDING_REGISTERS;
  packet5->address = 3109;
  packet5->no_of_registers = ARRSIZE(data5);
  packet5->register_array = data5;

  packet6->id = 1;
  packet6->function = READ_HOLDING_REGISTERS;
  packet6->address = 3203;
  packet6->no_of_registers = ARRSIZE(data6);
  packet6->register_array = data6;
  
  modbus_configure(baud, timeoutt, polling, retry_count, TxEnablePin, packets, TOTAL_NO_OF_PACKETS);  
}




// CONVERT DATA MODBUS TO FLOAT 
// ```````````````````````````````
union f_2uint { // CONVERT REGISTER TO FLOAT ==> I got this function from : https://industruino.com/blog/our-news-1/post/modbus-tips-for-industruino-26#blog_content
  float f;
  uint16_t i[2];
};
union f_2uint f_number;

union u_2u { // CONVERT REGISTER TO FLOAT ==> I got this function from : https://industruino.com/blog/our-news-1/post/modbus-tips-for-industruino-26#blog_content
  uint64_t uu;
  uint16_t i[4];
};
union u_2u uu_number;

float meterData[35]; // converted registers (float) will be stored here and send to server with http request
unsigned int meterIndex;

void convertData(Packet* pc, unsigned int dt[]){
  
  for(unsigned int i=0;i<pc->no_of_registers;i++){
    if(pc->address + i == 3206){
      uu_number.i[0] = dt[i];
      uu_number.i[1] = dt[i-1];
      uu_number.i[2] = dt[i-2];
      uu_number.i[3] = dt[i-3];
      Serial.printf("\n%d\n", uu_number.uu);
      ENN = uu_number.uu;
    }

    else{
      //RINTLN(String(pc->address+i)+" : "+String(data2[i]));
      if(pc->address + i == 3034)continue; // 3034 not use ==> check on PM5350 register list
      if((pc->address + i) % 2 == 0){ //Get even register ==> all register on list usage even
        f_number.i[0] = dt[i];
        f_number.i[1] = dt[i-1];
        meterData[meterIndex] = isnan(f_number.f)?0:f_number.f;
        PRINTLN(String(pc->address+i)+" : "+String(meterData[meterIndex]));
        // Serial.println(String(pc->address+i)+" : "+String(meterData[meterIndex]));

        switch(pc->address+i){
          case vab:
            VAB = meterData[meterIndex];
            Serial.printf("VAB : %.2f\n", VAB);
            break;

          case vbc:
            VBC = meterData[meterIndex];
            Serial.printf("VBC : %.2f\n", VBC);
            break;
            
          case vca:
            VCA = meterData[meterIndex];
            Serial.printf("VCA : %.2f\n", VCA);
            break;
            
          case vll:
            VLL = meterData[meterIndex];
            Serial.printf("VLL : %.2f\n", VLL);
            break;

          case ca:
            CA = meterData[meterIndex];
            Serial.printf("CA : %.2f\n", VLL);
            break;
            
          case cB:
            CB = meterData[meterIndex];
            Serial.printf("CB : %.2f\n", VLL);
            break;
            
          case cC:
            CC = meterData[meterIndex];
            Serial.printf("CC : %.2f\n", VLL);
            break;
            
          case cr:
            CR = meterData[meterIndex];
            Serial.printf("CR : %.2f\n", VLL);
            break;

          case apt:
            APT = meterData[meterIndex];
            Serial.printf("Power : %.2f\n", VLL);
            break;
          
          case rpt:
            RPT = meterData[meterIndex];
            Serial.printf("Reactive : %.2f\n", VLL);
            break;
          
          case appr:
            APP = meterData[meterIndex];
            Serial.printf("Apparent : %.2f\n", VLL);
            break;
          
          case pft:
            PFT = meterData[meterIndex];
            Serial.printf("PowerFactor : %.2f\n", VLL);
            break;

          case frq:
            FRQ = meterData[meterIndex];
            Serial.printf("Freq : %.2f\n", VLL);
            break;
            
            // APT, RPT, APP, PFT;
            // CA, CB, CC, CN, CG, CR;
            // VAB, VBC, VCA, VLL, VAN, VBN, VCN, VLN;
            
        }
        meterIndex++;
      }
    }
    
  }
}
void modbusData(){
  //if (connection_status != TOTAL_NO_OF_PACKETS)
    //Serial.println("\nModbus packet not valid...");
   
  PRINTLN("\nREGISTER DATA :");
  meterIndex = 0;
  convertData(packet1,data1);
  convertData(packet2,data2);
  convertData(packet3,data3);
  convertData(packet4,data4);
  convertData(packet5,data5);
  convertData(packet6,data6);
}
