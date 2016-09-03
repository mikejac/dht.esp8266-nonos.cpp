/* 
 * The MIT License (MIT)
 * 
 * ESP8266 Non-OS Firmware
 * Copyright (c) 2015 Michael Jacobsen (github.com/mikejac)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "dht22.hpp"
#include <github.com/mikejac/misc.esp8266-nonos.cpp/espmissingincludes.h>
#include <github.com/mikejac/esp-open-rtos.gpio.esp8266-nonos.cpp/gpio/gpio.h>
#include <osapi.h>

using namespace esp_nonos::dht;

#define DTXT(...)       os_printf(__VA_ARGS__)

#define MAXTIMINGS      10000
#define BREAKTIME       20

/******************************************************************************************************************
 * dht22_t
 *
 */

/**
 * 
 * @param gpio_num
 * @return 
 */
int ICACHE_FLASH_ATTR dht22_t::init(uint8_t gpio_num)
{
    DTXT("dht22_t::init(): begin\n");
    
    m_GpioNum = gpio_num;
    
    DTXT("dht22_t::init(): end\n");
    
    return 0;
}
/**
 * 
 * @param temp
 * @param hum
 * @return 
 */
int ICACHE_FLASH_ATTR dht22_t::read(double& temp, double& hum)
{
    int ret = 0;
    
    DTXT("dht22_t::read(): begin; m_GpioNum = %d\n", m_GpioNum);
    
    // clear checksum bytes
    m_Data[0] = m_Data[1] = m_Data[2] = m_Data[3] = m_Data[4] = 0;
  
    gpio_enable(m_GpioNum, GPIO_OUTPUT);
    gpio_write(m_GpioNum, true);
    
    countdown_ms(&m_Countdown, 250);
    while(!expired(&m_Countdown))
        ;
    
    gpio_write(m_GpioNum, false);

    countdown_ms(&m_Countdown, 20);
    while(!expired(&m_Countdown))
        ;
    
    gpio_write(m_GpioNum, true);
    gpio_enable(m_GpioNum, GPIO_INPUT_PULLUP);
    
    int i = 0;
    
    // wait for pin to drop
    while(gpio_read(m_GpioNum) == true && i < 100000) {
        os_delay_us(1);
        i++;
    }

    if(i == 100000) {
        DTXT("dht22_t::read(): error; i == 100000\n");
        return -1;
    }
    
    int  j         = 0;
    int  counter   = 0;
    bool laststate = true;
    
    //
    // now read incoming data stream
    //
    for(i = 0; i < MAXTIMINGS; i++) {
        counter = 0;
        
        while(gpio_read(m_GpioNum) == laststate) {
            counter++;
            os_delay_us(1);
            
            if(counter == 1000) {
                break;
            }
        }
        
        laststate = gpio_read(m_GpioNum);
    
        if(counter == 1000) {
            DTXT("dht22_t::read(): counter == 1000; j = %d\n", j);
            break;
        }

        // ignore first 3 transitions
        if((i > 3) && (i % 2 == 0)) {
            // shove each bit into the storage bytes
            m_Data[j / 8] <<= 1;
            
            if(counter > BREAKTIME) {
                m_Data[j / 8] |= 1;
            }
            
            j++;
        }
    }
    
    int checksum;
    
    //
    // do the math
    //
    if(j >= 39) {
        checksum = (m_Data[0] + m_Data[1] + m_Data[2] + m_Data[3]) & 0xFF;
        
        if(m_Data[4] == checksum) {     // is checksum is valid?
            hum = m_Data[0] * 256 + m_Data[1];
            hum /= 10;

            temp = (m_Data[2] & 0x7F) * 256 + m_Data[3];
            temp /= 10.0;
            
            if(m_Data[2] & 0x80) {
                temp *= -1;
            }
        }
        else {
            DTXT("dht22_t::read(): error; invalid checksum\n");
            ret = -2;
        }
    }
    else {
        DTXT("dht22_t::read(): error; j < 39\n");
        ret = -3;
    }
    
    DTXT("dht22_t::read(): end; ret = %d\n", ret);
    
    return ret;
}
