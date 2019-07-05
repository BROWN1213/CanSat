/**
* @file 
* @author SeungMin Shin, Haneulbit Kim, Chan Lee
* @license This project is released under the MIT License (MIT)
* @copyright Copyright (c) 2019 Asgardia
* @date June 2019
* @brief ...
*/

#ifndef CANSAT_BT_H
#define CANSAT_BT_H

#include "cansat_serial.h"
#include <Arduino.h>

template <class T>
class CansatBT
      : public CansatTransportSerial<T>
{
  public:
      CansatBT(T& stream)
          : CansatTransportSerial<T>(stream)
      {}


        bool begin() {

            this->stream.begin(9600);
            sendATcommand("+++\r", "OK", 20);
            sendATcommand("at+btmode,3\r", "OK", 20);
            sendATcommand("at+uartconfig,115200,N,1,0\r", "OK", 20);
            sendATcommand("atz\r", "OK", 20);
            this->stream.begin(115200);

            if(sendATcommand("at\r", "OK", 20)){
                return true;
            }else{
                return false;
            }

        }
        void send(char *a){
          rx_empty();
          this->stream.write(a,strlen(a));
        }

        void sendln(char *a){
          send(a);
          this->stream.write("\n",1);
        }
        void send(int a){
          rx_empty();
          char buf[10];
          memset(buf,'\0',10);
          itoa(a,buf,10);
          this->stream.write(buf,strlen(buf));
        }
        void sendln(int a){
          send(a);
          this->stream.write("\n",1);
        }
        void send(float a){
          rx_empty();
          char buf[20];
          memset(buf,'\0',20);
          dtostrf(a,15,4,buf);
          this->stream.write(buf,strlen(buf));
        }
        void sendln(float a){
          send(a);
          this->stream.write("\n",1);
        }
        void echo(){
          while(this->stream.available() > 0){
            char a;
            this->stream.read(&a,1);
            this->stream.write(&a,1);
          }
        }
        void rx_empty(void)
        {
            char a;
            while(this->stream.available() > 0) {
                this->stream.read(&a,1);
            }
        }
        String recvResponse(unsigned int timeout)
        {
            String data;
            char a;
            unsigned long start = millis();
            while (millis() - start < timeout) {
                while(this->stream.available() > 0) {
                    this->stream.read(&a,1);

                    data += a;
                    //if(a=="\n")break;
                }
            }
            return data;
        }
        String recvString(String target,  unsigned int timeout)
        {
            String data;
            char a;
            unsigned long start = millis();
            while (millis() - start < timeout) {
                while(this->stream.available() > 0) {
                    this->stream.read(&a,1);
                    data += a;
                    //Serial.write(a);
                }
                if (data.indexOf(target) != -1) {
                    break;
                }
            }
            return data;
        }
        bool recvFind(String target,  unsigned int timeout)
        {
            String data_tmp;
            data_tmp = recvString(target, timeout);
            if (data_tmp.indexOf(target) != -1) {
                return true;
            }
            return false;
        }
        bool recvFindAndFilter(String target, String begin, String end, String &data,  unsigned int timeout)
        {
            String data_tmp;
            data_tmp = recvString(target, timeout);
            if (data_tmp.indexOf(target) != -1) {
                int32_t index1 = data_tmp.indexOf(begin);
                int32_t index2 = data_tmp.indexOf(end);
                if (index1 != -1 && index2 != -1) {
                    index1 += begin.length();
                    data = data_tmp.substring(index1, index2);
                    return true;
                }
            }
            data = "";
            return false;
        }
        bool isConnected(){
            if(recvFind("CONNECT",10)){
                return true;
            }else{
                return false;
            }
        }

      int connected() { return this->conn && this->stream; }
      int available() { return this->stream.available(); }
    private:
      bool sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout){
        rx_empty();
        this->stream.write(ATcommand,strlen(ATcommand));
        return recvFind(expected_answer,timeout);

      }

      String sendRespATcommand(char* ATcommand, unsigned int timeout){
        rx_empty();
        this->stream.write(ATcommand,strlen(ATcommand));
        return recvResponse(timeout);

      }


};


typedef CansatTransportSerial<HardwareSerial> CansatHwSerial;


#endif