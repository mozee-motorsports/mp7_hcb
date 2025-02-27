/******************************************************************************
 * accelerometer_sensor.h
 *
 * Description:	I2C IO for accelerometer input with interrupt input.
 *
 * Needed Peripherals: I2C
 *
 *  Created on: Feb 9, 2025
 *      Author: nimsgernd
 *****************************************************************************/

// include guard
#ifndef INC_ACCELEROMETER_SENSOR_H_
#define INC_ACCELEROMETER_SENSOR_H_

// define this as extern for c++
#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
 |          Includes
 ============================================================================*/

#include <stdint.h>

/*=============================================================================
 |          Defines
 ============================================================================*/

#define ACCELEROMETER_ADDR	(uint8_t)	0x18

// Register addresses
#define ACC_CHIP_ID				(uint8_t)	0x00
#define ACC_HEALTH_STATUS		(uint8_t)	0x02
#define ACC_CMD_SUSPEND			(uint8_t)	0x04
#define ACC_CONFIG_STATUS		(uint8_t)	0x10
#define ACC_SENSOR_STATUS		(uint8_t)	0x11
#define ACC_INT_STATUS_INT1_0	(uint8_t)	0x12
#define	ACC_INT_STATUS_INT1_1	(uint8_t)	0x13
#define ACC_INT_STATUS_INT2_0	(uint8_t)	0x14
#define ACC_INT_STATUS_INT2_1	(uint8_t)	0x15
#define ACC_INT_STATUS_I3C_0	(uint8_t)	0x16
#define ACC_INT_STATUS_I3C_1	(uint8_t)	0x17
#define ACC_ACC_DATA_0			(uint8_t)	0x18
#define ACC_ACC_DATA_1			(uint8_t)	0x19
#define ACC_ACC_DATA_2			(uint8_t)	0x1A
#define ACC_ACC_DATA_3			(uint8_t)	0x1B
#define ACC_ACC_DATA_4			(uint8_t)	0x1C
#define ACC_ACC_DATA_5			(uint8_t)	0x1D
#define ACC_TEMP_DATA			(uint8_t)	0x1E
#define ACC_SENSOR_TIME_0		(uint8_t)	0x1F
#define ACC_SENSOR_TIME_1		(uint8_t)	0x20
#define ACC_SENSOR_TIME_2		(uitn8_t)	0x21
#define ACC_FIFO_LEVEL_0		(uint8_t)	0x22
#define ACC_FIFO_LEVEL_1		(uint8_t)	0x23
#define ACC_FIFO_DATA_OUT		(uint8_t)	0x24
#define ACC_ACC_CONF_0			(uint8_t)	0x30
#define ACC_ACC_CONF_1			(uint8_t)	0x31
#define ACC_ACC_CONF_2			(uint8_t)	0x32
#define ACC_TEMP_CONF			(uint8_t)	0x33
#define ACC_INT1_CONF			(uint8_t)	0x34
#define ACC_INT2_CONF			(uint8_t)	0x35
#define ACC_INT_MAP_0			(uint8_t)	0x36
#define ACC_INT_MAP_1			(uint8_t)	0x37
#define ACC_INT_MAP_2			(uint8_t)	0x38
#define ACC_INT_MAP_3			(uint8_t)	0x39
#define ACC_IF_CONF_0			(uint8_t)	0x3A
#define ACC_IF_CONF_1			(uint8_t)	0x3B
#define ACC_FIFO_CTRL			(uint8_t)	0x40
#define ACC_FIFO_CONF_0			(uint8_t)	0x41
#define ACC_FIFO_CONF_1			(uint8_t)	0x42
#define ACC_FIFO_WM_0			(uint8_t)	0x43
#define ACC_FIFO_WM_1			(uint8_t)	0x44
#define ACC_FEAT_ENG_CONF		(uint8_t)	0x50
#define ACC_FEAT_ENG_STATUS		(uint8_t)	0x51
#define ACC_FEAT_ENG_GP_FLAGS	(uint8_t)	0x52
#define ACC_FEAT_ENG_GPR_CONF	(uitn8_t)	0x53
#define ACC_FEAT_ENG_GPR_CTRL	(uint8_t)	0x54
#define ACC_FEAT_ENG_GPR_0		(uint8_t)	0x55
#define ACC_FEAT_ENG_GPR_1		(uint8_t)	0x56
#define ACC_FEAT_ENG_GPR_2		(uint8_t)	0x57
#define ACC_FEAT_ENG_GPR_3		(uint8_t)	0x58
#define ACC_FEAT_ENG_GPR_4		(uint8_t)	0x59
#define ACC_FEAT_ENG_GPR_5		(uint8_t)	0x5A
#define ACC_FEATURE_DATA_ADDR	(uitn8_t)	0x5E
#define ACC_FEATURE_DATA_TX		(uint8_t)	0x5F
#define ACC_ACC_OFFSET_0		(uint8_t)	0x70
#define ACC_ACC_OFFSET_1		(uint8_t)	0x71
#define ACC_ACC_OFFSET_2		(uint8_t)	0x72
#define ACC_ACC_OFFSET_3		(uint8_t)	0x73
#define ACC_ACC_OFFSET_4		(uint8_t)	0x74
#define ACC_ACC_OFFSET_5		(uint8_t)	0x75
#define ACC_ACC_SELF_TEST		(uint8_t)	0x76
#define ACC_CMD					(uint8_t)	0x7E

/*=============================================================================
 |          Enumerations
 ============================================================================*/



/*=============================================================================
 |          typeDefs
 ============================================================================*/



/*=============================================================================
 |          Function Prototypes
 ============================================================================*/



// end c++ guard
#ifdef __cplusplus
}
#endif

// end include guard
#endif /* INC_TEMPERTURE_SENSORS_H_ */
