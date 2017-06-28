#include "RaspberryI2CBus.h"
#include <stdio.h>


#define setbit(n,  x)  ((1<<n) | x )
#define clearbit(n, x)  ( (~(1<<n)) & x)

int main()
{
    RaspberryI2CBus i2c;
    unsigned char val;
    i2c.I2C_READ (0x4200, &val );
    printf("Exposure 0x4200: %02x\n", val);
    i2c.I2C_READ (0x4201, &val );
    printf("Exposure 0x4201: %02x\n", val);
    i2c.I2C_READ (0x4202, &val );
    printf("Exposure 0x4202: %02x\n", val);
    i2c.I2C_READ (0x4203, &val );
    printf("Exposure 0x4203: %02x\n", val);

    i2c.I2C_READ (0x3820, &val );
    printf("Exposure 0x3820: %02x\n", val);
    i2c.I2C_READ (0x3821, &val );
    printf("Exposure 0x3821: %02x\n", val);

    i2c.I2C_WRITE (0x3820, 0x40 );

    i2c.I2C_READ (0x3820, &val );
    printf("Exposure 0x3820: %02x\n", val);
    i2c.I2C_READ (0x3821, &val );
    printf("Exposure 0x3821: %02x\n", val);

    return 0;
    unsigned int exposure = 0;

    i2c.I2C_READ (0x3503, &val );
    printf("Exposure 0x3503: %02x\n", val);

    i2c.I2C_READ (0x3A00, &val );
    printf("Exposure 0x3A00: %02x\n", val);

    i2c.I2C_READ (0x3A01, &val );
    printf("Exposure 0x3A01: %02x\n", val);

    i2c.I2C_READ (0x3A02, &val );
    printf("Exposure 0x3A02: %02x\n", val);


    i2c.I2C_READ (0x3A03, &val );
    printf("Exposure 0x3A03: %02x\n", val);

    i2c.I2C_READ (0x3A00, &val );
    printf("Exposure 0x3A00: %02x\n", val);


    i2c.I2C_READ (0x3500, &val );
    printf("Exposure: %02x\n", val);

    i2c.I2C_READ (0x3501, &val );
    printf("Exposure: %02x\n", val);

    i2c.I2C_READ (0x3502, &val );
    printf("Exposure: %02x\n", val);

    i2c.I2C_WRITE (0x3500, 0x00 );
    i2c.I2C_WRITE (0x3501, 0x20 );
    i2c.I2C_WRITE (0x3502, 0x20 );

    i2c.I2C_READ (0x3A20, &val );
    printf("Exposure 0x3A20: %02x\n", val);
    i2c.I2C_READ (0x3A21, &val );
    printf("Exposure 0x3A21: %02x\n", val);

    return 0;

    i2c.I2C_READ(0x3500, &val );
    printf("%x\n", val);
    i2c.I2C_READ(0x3501, &val );
    printf("%x\n", val);
    i2c.I2C_READ(0x3502, &val );
    printf("%x\n", val);

    i2c.I2C_WRITE (0x3500, 0x00);
    i2c.I2C_WRITE (0x3501, 0x20);
    i2c.I2C_WRITE (0x3502, 0x00);
    //if( i2c.I2C_READ(0x3B07, &val ) >=0)
    //printf("%x\n", val);

    val = 0x08;
    i2c.I2C_WRITE (0x3B04, val);
    val = 1;
    if( i2c.I2C_READ(0x3B04, &val ) >=0)
        printf("%x\n", val);
    return 0;

}
