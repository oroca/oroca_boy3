/*
 * mpu.h
 *
 *  Created on: 2019. 11. 5.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_INCLUDE_MPU_H_
#define SRC_COMMON_HW_INCLUDE_MPU_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_MPU



bool mpuInit(void);

#endif


#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_INCLUDE_MPU_H_ */
