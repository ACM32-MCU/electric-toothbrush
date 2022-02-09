#ifndef		__INERTIAL_CHIP_H__
#define		__INERTIAL_CHIP_H__

#include "ACM32F0x0.h"

/**
		This file is use simulator IIC interface to communicate with QMI8658C, an 
	inertial measurement chip.
**/

/**@defgroup	ID Address Defination
 \@{
*/

#define		ADDR_DEV_ID				0x00
#define		ADDR_REV_ID				0x01

// Control Register Address
#define		ADDR_CTRL_1				0x02
#define		ADDR_CTRL_2				0x03
#define		ADDR_CTRL_3				0x04
#define		ADDR_CTRL_4				0x05
#define		ADDR_CTRL_5				0x06
#define		ADDR_CTRL_6				0x07
#define		ADDR_CTRL_7				0x08
#define		ADDR_CTRL_8				0x09
#define		ADDR_CTRL_9				0x0A



/**
 \@}
*/


/**@defgroup	Configuration Defination
 \@{
*/

// Accelerometer Configure Parameter		addr: 0x03
#define		ACCE_SELF_TEST_MSK		0x80
#define		ACCE_SELF_TEST_POS		7UL
#define		ACCE_SELF_TEST_EN		0x80

#define		ACCE_FULL_SCALE_MSK		0x70
#define		ACCE_FULL_SCALE_POS		4UL

#define		ACCE_FULL_SCALE_2G		0x00
#define		ACCE_FULL_SCALE_4G		0x01
#define		ACCE_FULL_SCALE_8G		0x02
#define		ACCE_FULL_SCALE_16G		0x03

#define		ACCE_ODR_MSK			0x0F
#define		ACCE_ODR_POS			0UL

#define		ACCE_ODR_8000			0x00
#define		ACCE_ODR_4000			0x01
#define		ACCE_ODR_2000			0x02
#define		ACCE_ODR_1000			0x03
#define		ACCE_ODR_500			0x04
#define		ACCE_ODR_250			0x05
#define		ACCE_ODR_125			0x06
#define		ACCE_ODR_62_5			0x07
#define		ACCE_ODR_31_25			0x08
#define		ACCE_ODR_128			0x0C
#define		ACCE_ODR_21				0x0D
#define		ACCE_ODR_11				0x0E
#define		ACCE_ODR_3				0x0F

//	Gyroscope Configure Parameter			addr: 0x04
#define		GYRO_SELF_TEST_MSK		0x80
#define		GYRO_SELF_TEST_POS		7UL
#define		GYRO_SELF_TEST_EN		0x80

#define		GYRO_FULL_SCALE_MSK		0x70
#define		GYRO_FULL_SCALE_POS		4UL

#define		GYRO_FULL_SCALE_16		0x00
#define		GYRO_FULL_SCALE_32		0x01
#define		GYRO_FULL_SCALE_64		0x02
#define		GYRO_FULL_SCALE_128		0x03
#define		GYRO_FULL_SCALE_256		0x04
#define		GYRO_FULL_SCALE_512		0x05
#define		GYRO_FULL_SCALE_1024	0x06
#define		GYRO_FULL_SCALE_2048	0x07

#define		GYRO_ODR_MSK			0x0F
#define		GYRO_ODR_POS			0UL

#define		GYRO_ODR_8000			0x00
#define		GYRO_ODR_4000			0x01
#define		GYRO_ODR_2000			0x02
#define		GYRO_ODR_1000			0x03
#define		GYRO_ODR_500			0x04
#define		GYRO_ODR_250			0x05
#define		GYRO_ODR_125			0x06
#define		GYRO_ODR_62_5			0x07
#define		GYRO_ODR_31_25			0x08

// Magnetometer Configure Parameter			addr: 0x05
#define		MAGN_DEV_CFG_MSK		0x78
#define		MAGN_DEV_CFG_POS		3UL


#define		MAGN_ODR_MSK			0x07
#define		MAGN_ODR_POS			0UL

#define		MAGN_ODR_1000			0x00
#define		MAGN_ODR_500			0x01
#define		MAGN_ODR_250			0x02
#define		MAGN_ODR_125			0x03
#define		MAGN_ODR_62_5			0x04
#define		MAGN_ODR_31_25			0x05

// Accelerometer and Gyroscope Low-Pass Filter configure		addr: 0x06
#define		MAGN_LPF_MODE_MSK		0x60
#define		MAGN_LPF_MODE_POS		5UL

#define		MAGN_LPF_MODE_2_62		0x00
#define		MAGN_LPF_MODE_3_59		0x01
#define		MAGN_LPF_MODE_5_32		0x02
#define		MAGN_LPF_MODE_14		0x03

#define		MAGN_LPF_EN_MSK			0x10
#define		MAGN_LPF_EN_POS			4UL

#define		MAGN_LPF_EN				0x10

#define		ACCE_LPF_MODE_MSK		0x06
#define		ACCE_LPF_MODE_POS		1UL

#define		ACCE_LPF_MODE_2_62		0x00
#define		ACCE_LPF_MODE_3_59		0x01
#define		ACCE_LPF_MODE_5_32		0x02
#define		ACCE_LPF_MODE_14		0x03

#define		ACCE_LPF_EN_MSK			0x01
#define		ACCE_LPF_EN_POS			0UL

#define		ACCE_LPF_EN				0x01

// Attitude Engine ODR and Motion On Demand			addr: 0x07
#define		ATTI_ENG_EN_MSK			0x80
#define		ATTI_ENG_EN_POS			7UL
#define		ATTI_ENG_EN				0x80

#define		ATTI_ENG_ODR_MSK		0x07
#define		ATTI_ENG_ODR_POS		0UL

#define		ATTI_ENG_ODR_1			0x00
#define		ATTI_ENG_ODR_2			0x01
#define		ATTI_ENG_ODR_4			0x02
#define		ATTI_ENG_ODR_8			0x03
#define		ATTI_ENG_ODR_16			0x04
#define		ATTI_ENG_ODR_32			0x05
#define		ATTI_ENG_ODR_64			0x06

// Enable Sensors and Configure Data Reads			addr: 0x08
#define		SYNC_SMPL_EN_MSK		0x80
#define		SYNC_SMPL_EN_POS		7UL
#define		SYNC_SMPL_EN_EN			0x80

#define		SYS_HSIC_MSK			0x40
#define		SYS_HSIC_POS			6UL
#define		SYS_HSIC_EN				0x40

#define		GYNO_SNOOZE_MODE_MSK	0x10
#define		GYNO_SNOOZE_MODE_POS	4UL
#define		GYNO_SNOOZE_MODE_EN		0x10

#define		ATTI_EN_MSK				0x08
#define		ATTI_EN_POS				3UL
#define		ATTI_EN					0x08

#define		MAGN_EN_MSK				0x04
#define		MAGN_EN_POS				2UL
#define		MAGN_EN					0x04

#define		GYRO_EN_MSK				0x02
#define		GYRO_EN_POS				1UL
#define		GYRO_EN					0x02

#define		ACCE_EN_MSK				0x01
#define		ACCE_EN_POS				0UL
#define		ACCE_EN					0x01

/**
 \@}
*/


void QMI8658_Test(void);

void EEPROM_24LC256_Test(void);

#endif





