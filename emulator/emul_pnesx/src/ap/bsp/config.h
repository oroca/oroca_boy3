/*
 * config.h
 *
 *  Created on: 2019. 11. 5.
 *      Author: Baram
 */

#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_


#define FLASH_ADDR_TAG                0x24000000


#define FLASH_ADDR_FW                 (FLASH_ADDR_TAG + 0x400)

#define FLASH_ADDR_START              FLASH_ADDR_TAG
#define FLASH_ADDR_END                (FLASH_ADDR_START + 2048*1024)


#endif /* SRC_CONFIG_H_ */
