# ESP32_UART_DMA
This library utilize ESP IDF DMA UART but compatible with Stream class in arduino, Perfect for AlfredoCRSF or other library that use UART and Stream class but it need fast and low cpu usage UART

> [!CAUTION]
> SOME OF THE FUNCTION STILL NOT IMPLEMENTED 
> PEEK FUNCTION IS ALWAYS RETURN -1 (there is no peak function for dma)
> Avalable for write is returning buffer for dma



 ### Supported ESP32 Version

| IDE         | ESP32 Board Version |
| ----------- | ------------------- |
| Arduino IDE | `>= 2.0.11`         |
| PlatformIO  | `>= 5.0.0`          |

## API INFORMATION
### SETUP
```C++
    // to set port 
    // port 0 , 1 , 2
    // note if possible dont use port  0 because it for JTAG and USB serial
    SerialEsp32DMA(uart_port_t port);

    //OPTIONAL 
    // set up buffer for both rx and tx
    // the default is 1024
    (void) setBuffer(unsigned int bufferSize);

    // to begin or start and setup your UART PORT
    (void) begin(int baudrate,uint32_t config, uint8_t rx, uint8_t tx);

    // to end all comunication
    (void) end();

    //you can check if it working or not by 
    SerialEsp32DMA uart(1);

    // 0 fail
    // 1 working
    if(uart){
        Serial.println(" it working ")
    }

```
### STREAM COMPATIBLE
```C++ 
    // to check if it available or not 
    (int) available(void);

    // returning set max buffer
    (int) availableForWrite(void);
    
    //read
    (int) read(void);

    //dont use this because it always return -1 
    (int) peek(void);   // always return to -1

    // wait to all transmision (tx)
    (void) flush(void);

    //write 
    (size_t) write(uint8_t c);
    (size_t) write(const uint8_t *c, size_t size);  
    (size_t) write(const char *c);
    (size_t) write(const char *c, size_t size) ;  
```

> [!TIP]
> it also compatible using arduino lib base enum for set the port and the config
