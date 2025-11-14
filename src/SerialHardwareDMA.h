#pragma once

#ifndef SERIAL_DMA
#define SERIAL_DMA

/*
____________________________________________________________________________________________________________________
Library for UART SERIAL COMMUNICATION utilizing ESP_IDF DMA function and still supported under arduino Stream class

- This library sill not supported all Stream class but it have basic like write and read function

- Some of class is not working due limitation of ESP_IDF for dma but still writen for safety reason

- avalable for write it return your max buffer setting or default buffer provided by this library 



This code write by Sekartalok 
____________________________________________________________________________________________________________________
*/
#define TAG "SerialHardwareDMA"

#include <Arduino.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"


class SerialEsp32DMA : public Stream {
public:
    SerialEsp32DMA(uart_port_t port);
    ~SerialEsp32DMA();
   
    operator bool() const {
        return isInitialize;
    }
    void setBuffer(unsigned int bufferSize);
    void begin(int baudrate,uint32_t config, uint8_t rx, uint8_t tx);
    void end();

    //compatible Stream IO 
    virtual int available(void) override;
    virtual int availableForWrite(void) override;
    virtual int read(void) override;
    virtual int peek(void) override;   // always return to 0 
    virtual void flush(void) override;
    virtual size_t write(uint8_t c) override;
    size_t write(const uint8_t *c, size_t size);  
    virtual size_t write(const char *c);
    size_t write(const char *c, size_t size) ;  
    
private:

    int baudrate;
    uint32_t config;
    uint8_t rx;
    uint8_t tx;
    uart_port_t port;
    bool isInitialize{false};
    unsigned int bufferSize{1024};
    unsigned int _timeout {1000};

    //function helper 
    void converter(uint8_t *parityBit, uint8_t *dataBit, uint8_t *stopBit);
    bool initialize();
};

#endif