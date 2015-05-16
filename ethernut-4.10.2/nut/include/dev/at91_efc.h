#ifndef _DEV_AT91_EFC_H_
#define _DEV_AT91_EFC_H_

#include <sys/types.h>

/*!
 * \brief Load data from MCU on-chip flash.
 *
 * \return 0 on success, -1 otherwise.
 */
extern int OnChipFlashLoad(uint32_t addr, void *buff, size_t siz);

/*!
 * \brief Save data to MCU on-chip flash.
 *
 * \return 0 on success, -1 otherwise.
 */
extern int OnChipFlashSave(uint32_t addr, CONST void *buff, size_t len);

extern int FlashAreaIsLock(uint32_t addr);

extern int LockFlashArea(uint32_t addr);

extern int UnlockFlashArea(uint32_t addr);

#endif
