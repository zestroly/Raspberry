#include "RaspberryI2CBus.h"
#include <stdio.h>
#include <linux/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>


RaspberryI2CBus::RaspberryI2CBus()
{
        fd = open("/dev/i2c-0", O_RDWR);
        ioctl(fd, I2C_TIMEOUT , 1);
        ioctl(fd, I2C_RETRIES, 3);
        usleep(2000);
}


int RaspberryI2CBus::I2C_READ(unsigned short Reg, unsigned char *Val)
{
    if(fd < 0)
        return -1;
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];
    unsigned char tempReg[2] = {0};
    tempReg[0] = ((Reg >> 8) & 0xFF);
    tempReg[1] = ((Reg & 0xFF));
    packets.nmsgs = 2;
    //发送要读取的寄存器地址
    messages[0].addr = 0x36;
    messages[0].flags = 0;              //write
    messages[0].len = 2;                //数据长度
    messages[0].buf = tempReg;          //发送寄存器地址

    //读取数据
    messages[1].len = 1;                           //读取数据长度
    messages[1].addr = 0x36;                         //设备地址
    messages[1].flags = I2C_M_RD;                    //read
    messages[1].buf = Val;

    packets.msgs = messages;

    while (ioctl(fd, I2C_RDWR, (unsigned long)&packets) < 0);
    return 0;
}

int RaspberryI2CBus::I2C_WRITE(unsigned short Reg, unsigned char Val)
{
    if(fd < 0)
        return -1;
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages;
    unsigned char tempdata[3] ={0};
    tempdata[0] = ((Reg >> 8) & 0xFF);
    tempdata[1] = ((Reg & 0xFF));
    tempdata[2] = Val;

    packets.nmsgs = 1;
    //发送要读取的寄存器地址
    messages.addr = 0x36;
    messages.flags = 0;              //write
    messages.len = 3;                //数据长度
    messages.buf = tempdata;          //发送寄存器地址

    packets.msgs = &messages;
    while (ioctl(fd, I2C_RDWR, (unsigned long)&packets) < 0);
    return 0;
}

RaspberryI2CBus::~RaspberryI2CBus()
{
    close(fd);
}
