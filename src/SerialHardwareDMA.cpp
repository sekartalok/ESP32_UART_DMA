#include "SerialHardwareDMA.h"
/*

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/


SerialEsp32DMA::SerialEsp32DMA(uart_port_t port){
    this->port = port;
}

SerialEsp32DMA::~SerialEsp32DMA(){
    //to make sure that the class is destroy safely
    end();
}

// Begin and setting for the UART DMA 
void SerialEsp32DMA::setBuffer(unsigned int bufferSize){
    this->bufferSize =bufferSize;
}

void SerialEsp32DMA::begin(int baudrate,uint32_t config, uint8_t rx, uint8_t tx) {

    this->config = config;
    this->baudrate = baudrate;
    this->rx = rx;
    this->tx = tx;
    isInitialize = initialize();
}

void SerialEsp32DMA::end() {
    uart_driver_delete(port); 
}

//convert it from standar arduino hex to usable for DMA setting config

/*
   SerialConfig Bit Layout Explanation:

   0 1 2 3 4 5 ... 22 23
   | | | | | |      |
   | | | | | |      +--> Bit 23: reserved/internal flag (0x800000)
   | | | | | +---------> Stop bits
   | | | | |              0 = 1 stop bit
   | | | | |              2 = 2 stop bits
   | | | +---------------> Data bits
   | | |                  0 = 5 bits
   | | |                  1 = 6 bits
   | | |                  2 = 7 bits
   | | |                  3 = 8 bits
   | | +-----------------> Parity
   | |                      0 = None (N)
   | |                      2 = Even (E)
   | |                      3 = Odd (O)
   | +---------------------> Not used
   +-----------------------> Not used

   Example: SERIAL_8N1 = 0x800001C
   - Parity bits 0-1 = 0 (N)
   - Data bits 2-4  = 3 (8 bits)
   - Stop bits 4-5  = 0 (1 stop bit)
   - Bit 23 = 1 (reserved flag)
*/

void SerialEsp32DMA::converter(uint8_t *parityBit, uint8_t *dataBit, uint8_t *stopBit) {
    *parityBit = config & 0x03;
    *dataBit = (config >> 2) & 0x07;
    *stopBit = (config >> 4) & 0x03;
}

bool SerialEsp32DMA::initialize() {
    uint8_t dataBit = 0x00;
    uint8_t parityBit = 0x00;
    uint8_t stopBit = 0x00;

    converter(&parityBit, &dataBit, &stopBit);

    uart_config_t uart_config = {
        .baud_rate = baudrate,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    switch (dataBit) {
        case 0: uart_config.data_bits = UART_DATA_5_BITS; break;
        case 1: uart_config.data_bits = UART_DATA_6_BITS; break;
        case 2: uart_config.data_bits = UART_DATA_7_BITS; break;
        case 3: uart_config.data_bits = UART_DATA_8_BITS; break;
        default: uart_config.data_bits = UART_DATA_8_BITS; break;
    }

    switch (parityBit) {
        case 0: uart_config.parity = UART_PARITY_DISABLE; break;
        case 2: uart_config.parity = UART_PARITY_EVEN; break;
        case 3: uart_config.parity = UART_PARITY_ODD; break;
        default: uart_config.parity = UART_PARITY_DISABLE; break;
    }

    switch (stopBit) {
        case 0: uart_config.stop_bits = UART_STOP_BITS_1; break;
        case 2: uart_config.stop_bits = UART_STOP_BITS_2; break;
        default: uart_config.stop_bits = UART_STOP_BITS_1; break;
    }

    esp_err_t err;
    err = uart_param_config(port, &uart_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "uart_param_config failed: %s", esp_err_to_name(err));
        return false;
    }

    err = uart_set_pin(port, tx, rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "uart_set_pin failed: %s", esp_err_to_name(err));
        return false;
    }

    err = uart_driver_install(port, bufferSize, bufferSize, 0, NULL, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "uart_driver_install failed: %s", esp_err_to_name(err));
        return false;
    }


    return true;
}


// ============ Stream-compatible I/O functions ============

int SerialEsp32DMA::available() {
    size_t length = 0;
    uart_get_buffered_data_len(port, &length);
    return (int)length;
}

int SerialEsp32DMA::availableForWrite(void){
    //it return buffer size for both rx and tx 
    return bufferSize;
}

int SerialEsp32DMA::read() {
    uint8_t byte;
    int len = uart_read_bytes(port, &byte, 1, 0);
    if (len > 0) return byte;
    return -1;
}

int SerialEsp32DMA::peek() {
    //no peek function in DMA 
    return -1;
}

size_t SerialEsp32DMA::write(uint8_t c) {
    int r = uart_write_bytes(port, (const char*)&c, 1);
    return (r < 0) ? 0 : (size_t)r;
    
}

size_t SerialEsp32DMA::write(const uint8_t *c, size_t lenght){

    if(c == nullptr){return 0 ;}
    int r = uart_write_bytes(port,(const char*)c,lenght);
    return (r < 0) ? 0 : (size_t)r;
}

size_t SerialEsp32DMA::write(const char *c){

    if(c == nullptr){return 0 ;}
    size_t lenght = strlen(c);
    int r = uart_write_bytes(port,c,lenght);
    return (r < 0) ? 0 : (size_t)r;
}

size_t SerialEsp32DMA::write(const char *c, size_t lenght){
    if(c == nullptr){return 0;}
    int r = uart_write_bytes(port,c,lenght);
    return (r < 0) ? 0 : (size_t)r;
}

void SerialEsp32DMA::flush(){
    uart_wait_tx_done(port, portMAX_DELAY);
}



