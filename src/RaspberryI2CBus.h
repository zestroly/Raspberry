#ifndef RASPBERRYI2CBUS_H
#define RASPBERRYI2CBUS_H


class RaspberryI2CBus
{
public:
    RaspberryI2CBus();
    ~RaspberryI2CBus();
    int I2C_READ(unsigned short Reg, unsigned char *Val);
    int I2C_WRITE(unsigned short Reg, unsigned char Val);
private:
    int fd;
};

#endif // RASPBERRYI2CBUS_H
