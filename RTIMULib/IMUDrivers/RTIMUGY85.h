////////////////////////////////////////////////////////////////////////////
//
//  This file is part of RTIMULib
//
//  Copyright (c) 2014-2015, richards-tech, LLC
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of
//  this software and associated documentation files (the "Software"), to deal in
//  the Software without restriction, including without limitation the rights to use,
//  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
//  Software, and to permit persons to whom the Software is furnished to do so,
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//
//  03/11/2016 Jeff Loughlin
//  This module adds support for an IMU containing an HMC5883L magnetometer, ADXL345
//  accelerometer, and L3G4200D gyro.
//
//
#ifndef _RTIMUGY85_H
#define	_RTIMUGY85_H

#include "RTIMU.h"


#define ADXL345_ADDRESS 0x53

#define ITG3205_ADDRESS 0x68 //gy85

//#define HMC5883L_ADDRESS 0x1e
//ITG3205  reg
#define ITGCTRL_REG1 0x3E //power manager
#define ITGCTRL_REG2 0x15 //sample rate
#define ITGCTRL_REG3 0x17 //interrupt
#define ITGCTRL_REG4 0x16  // CTRL_REG4 controls the full-scale range, among other things:

class RTIMUGY85 : public RTIMU
{
public:
    RTIMUGY85(RTIMUSettings *settings);
    ~RTIMUGY85();

    virtual const char *IMUName() { return "L3G4200D + HMC5883L + ITG3202"; }
    virtual int IMUType() { return RTIMU_TYPE_GY85; }
    virtual bool IMUInit();
    virtual int IMUGetPollInterval();
    virtual bool IMURead();

private:
    bool setGyroSampleRate();
    bool readADXL345(short &x , short &y, short &z);
    bool readITG3200(short &x , short &y, short &z);
    bool readHMC5883L(short &x , short &y, short &z);
	
    unsigned char m_gyroSlaveAddr;                          // I2C address of L3G4200D gyro
    unsigned char m_accelSlaveAddr;                         // I2C address of ADXL345 accel
    unsigned char m_compassSlaveAddr;                       // I2C address of HMC5883L compass

    RTFLOAT m_gyroScale;
    RTFLOAT m_accelScale;
    RTFLOAT m_compassScale;
};

#endif // _RTIMUHMC5883LADXL345_H
