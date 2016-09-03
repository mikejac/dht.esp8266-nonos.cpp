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

#ifndef DHT22_HPP
#define	DHT22_HPP

#include <github.com/mikejac/timer.esp8266-nonos.cpp/timer.h>

namespace esp_nonos {
namespace dht {
    
/******************************************************************************************************************
 * dht22_t
 *
 */
class dht22_t
{
public:
    /**
     * 
     */
    dht22_t()
    {}
    /**
     * 
     * @param gpio_num
     * @return 
     */
    int init(uint8_t gpio_num);
    /**
     * 
     * @param temp
     * @param hum
     * @return 
     */
    int read(double& temp, double& hum);
    
private:
    uint8_t m_GpioNum;
    
    int     m_Data[100];
    
    Timer   m_Countdown;
    
    // no copy and no = operator
    dht22_t(const dht22_t&);
    dht22_t &operator=(const dht22_t&);
};

} //namespace dht {
} //namespace esp_nonos {

#endif	/* DHT22_HPP */

