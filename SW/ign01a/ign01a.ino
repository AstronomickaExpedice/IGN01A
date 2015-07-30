#include <Wire.h>

#define TIMEOUT_START 3000
#define TIMEOUT_MAX 20000
#define ALT_THRESHOLD 3.0

#define MPL3115A2 0x60 // Unshifted 7-bit I2C address for sensor
#define STATUS     0x00
#define OUT_P_MSB  0x01
#define OUT_P_CSB  0x02
#define OUT_P_LSB  0x03
#define OUT_T_MSB  0x04
#define OUT_T_LSB  0x05
#define DR_STATUS  0x06
#define OUT_P_DELTA_MSB  0x07
#define OUT_P_DELTA_CSB  0x08
#define OUT_P_DELTA_LSB  0x09
#define OUT_T_DELTA_MSB  0x0A
#define OUT_T_DELTA_LSB  0x0B
#define WHO_AM_I   0x0C
#define F_STATUS   0x0D
#define F_DATA     0x0E
#define F_SETUP    0x0F
#define TIME_DLY   0x10
#define SYSMOD     0x11
#define INT_SOURCE 0x12
#define PT_DATA_CFG 0x13
#define BAR_IN_MSB 0x14
#define BAR_IN_LSB 0x15
#define P_TGT_MSB  0x16
#define P_TGT_LSB  0x17
#define T_TGT      0x18
#define P_WND_MSB  0x19
#define P_WND_LSB  0x1A
#define T_WND      0x1B
#define P_MIN_MSB  0x1C
#define P_MIN_CSB  0x1D
#define P_MIN_LSB  0x1E
#define T_MIN_MSB  0x1F
#define T_MIN_LSB  0x20
#define P_MAX_MSB  0x21
#define P_MAX_CSB  0x22
#define P_MAX_LSB  0x23
#define T_MAX_MSB  0x24
#define T_MAX_LSB  0x25
#define CTRL_REG1  0x26
#define CTRL_REG2  0x27
#define CTRL_REG3  0x28
#define CTRL_REG4  0x29
#define CTRL_REG5  0x2A
#define OFF_P      0x2B
#define OFF_T      0x2C
#define OFF_H      0x2D

char i2c_write(char dev, char addr, char val)
{
    Wire.beginTransmission(dev);
    Wire.write(addr);
    Wire.write(val);
    Wire.endTransmission(true);
}

char i2c_read(char dev,char addr)
{
    Wire.beginTransmission(dev);
    Wire.write(addr);
    Wire.endTransmission(false);
    Wire.requestFrom(dev, 1);
    return Wire.read();
}

float alt_read(int samples)
{
    float alt = 0.0;
    int i = 0;
    for(i = samples; i >= 0; i--){
        i2c_write(MPL3115A2, CTRL_REG1, 0x82);
        delay(8);

        int amsb = (signed char) i2c_read(MPL3115A2, OUT_P_MSB);
        int acsb = (unsigned char) i2c_read(MPL3115A2, OUT_P_CSB);
        int alsb = (unsigned char) i2c_read(MPL3115A2, OUT_P_LSB);
    
        alt += (amsb * 256 + acsb) + (float) alsb / 256;
    }
    return alt / (samples + 1);
}

float temp_read()
{
    char tmsb = i2c_read(MPL3115A2, OUT_T_MSB);
    char tlsb = i2c_read(MPL3115A2, OUT_T_LSB);
    
    word complement = ~((tmsb << 8) + tlsb) + 1;
    tmsb = complement >> 8;
    tlsb = complement & 0xf0;
    return -((float)(tmsb + (tlsb >> 4) / 16.0));

}


long sitting = 0;

void setup() {
    Wire.begin();
    Serial.begin(115200);
    
    pinMode(2, OUTPUT);
    pinMode(3, INPUT);
    digitalWrite(2, HIGH);

    while(digitalRead(3)) ;
    sitting = millis();
}

float alt_max = 0.0;

void loop() {
    if ((millis() - sitting) > TIMEOUT_MAX){
        Serial.println("EJECT T");
        digitalWrite(2, LOW);
    }

    float alt = alt_read(10);

    if(alt > alt_max) {
        alt_max = alt;
    }    

    if((alt < alt_max) && ((millis() - sitting) > TIMEOUT_START)) {
        if((alt_max - alt) > ALT_THRESHOLD) {
            Serial.println("EJECT A");
            digitalWrite(2, LOW);
        }
    }
    
    Serial.print(alt, DEC);
    Serial.print(" m ");
    Serial.print(alt_max);
    Serial.print(" MAX ");
    Serial.print(alt_max - alt, DEC);
    Serial.print(" DIFF\n\r");
}
