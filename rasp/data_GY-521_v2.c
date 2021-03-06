#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdint.h>
#include <math.h>

/* IMU Data */
int16_t tempRaw;
int16_t gyroX;
int16_t gyroY;
int16_t gyroZ;
double accelX;
double accelY;
double accelZ;
int16_t gyroX_out;
int16_t gyroY_out;
int16_t gyroZ_out;
int16_t accelX_out;
int16_t accelY_out;
int16_t accelZ_out;


int read_value_i2c(int fd, int addres_register)
{
    int16_t value = read_value(fd, addres_register);
    if (value >= 0x8000)
    {
        return -((65535 - value) + 1);
    }
    else
    {
        return value;
    }
}

int read_value(fd, addres_register)
{
    int high = wiringPiI2CReadReg16(fd, addres_register);
    int low = wiringPiI2CReadReg16(fd, addres_register + 1);
    int value = (high << 8) + low;
    return value;
}

double dist(double a, double b)
{
    return sqrt((a*a)+(b*b));
}

double get_y_rotation(double x, double y, double z)
{
    double radians = atan2(x, dist(y,z));
    return -1*(180.0/3.14*radians);
}

double get_x_rotation(double x, double y, double z)
{
    double radians = atan2(y, dist(x,z));
    return 180.0/3.14*radians;
}

int main (int argc, char *argv[])
{
        int fd;
        int data;
        wiringPiSetup () ;
        fd = wiringPiI2CSetup (0x68);  /*Use i2c detect command to find your respective device address*/
        if(fd==-1)
        {
                printf("Can't setup the I2C device\n");
                return -1;
        }
        else
        {       
                wiringPiI2CWriteReg16(fd, 0x6b, 0x00); /*register 107 by datasheet -power management*/
                for (;;)
                {
                        data=wiringPiI2CRead(fd);
                        gyroX_out = read_value_i2c(fd, 0x43);
                        gyroX = gyroX_out/131;
                        gyroY_out = read_value_i2c(fd, 0x45);
                        gyroY = gyroY_out/131;
                        gyroZ_out = read_value_i2c(fd, 0x47);
                        gyroZ = gyroZ_out/131;
                        
                        accelX_out = read_value_i2c(fd, 0x3b);
                        accelX = (double)accelX_out / 16384.0;
                        accelY_out = read_value_i2c(fd ,0x3d);
                        accelY = (double)accelY_out / 16384.0;
                        accelZ_out = read_value_i2c(fd, 0x3f);
                        accelZ = (double)accelZ_out / 16384.0;

                        double X_rotation = get_x_rotation(accelX, accelY, accelZ);
                        double Y_rotation = get_y_rotation(accelX, accelY, accelZ);
				
                        if(data==-1)
                        {
                                printf("No data\n");
                                //return -1;
                        }
                        else
                        {
                                //print data

                                printf("X rotation:%f\n", X_rotation);
				                printf("Y rotation:%f\n", Y_rotation);
                                printf("    accelX:%f  ",accelX);
                                printf("    accelY:%f  ",accelY);
                                printf("    accelZ:%f    \n",accelZ);
                                printf("accelX_out:%d    ",accelX_out);
                                printf("     accelY_out%d     ",accelY_out);
                                printf("     accelZ_out%d     \n",accelZ_out);
				printf("    gyroX:%d  ",gyroX_out);
				printf("    gyroY:%d  ",gyroY_out);
				printf("    gyroZ:%d  \n",gyroZ_out);
                                printf(" ------------------------------------------------- \n");
				
                        }
                delay(1000);
                }
        }
return 0;
}
