/*
 * pka_fw.h
 *
 *  Created on: Aug 4, 2016
 *      Author: bobz
 */

#ifndef SRC_PKA_FW_H_
#define SRC_PKA_FW_H_
#ifndef PKA_RAM_FW
#define PKA_RAM_FW_SIZE	0
#else
#define PKA_RAM_FW_SIZE	834
#endif
extern unsigned int pka_fw[PKA_RAM_FW_SIZE];

#endif /* SRC_PKA_FW_H_ */
