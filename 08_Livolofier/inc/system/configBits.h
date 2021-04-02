#ifndef CONFIGBITS_H_0ec99d849f5f47efa0c4ec428b5e7358
#define CONFIGBITS_H_0ec99d849f5f47efa0c4ec428b5e7358

#include <inttypes.h>

// Config bits, defined for different chips.
// Currently for a PIC32MX270 target, and a PIC32MX440 target

#if defined(__32MX270F256D__) || defined(__32MX440F256H__)
extern const uint32_t temp3;
extern const uint32_t temp2;
extern const uint32_t temp1;
extern const uint32_t temp0;
#elif defined(__32MM0064GPL028__)
extern const uint32_t temp_fdevopt;
extern const uint32_t temp_ficd;
extern const uint32_t temp_fpor;
extern const uint32_t temp_fwdt;
extern const uint32_t temp_foscsel;
extern const uint32_t temp_fsec;
extern const uint32_t temp_afdevopt;
extern const uint32_t temp_aficd;
extern const uint32_t temp_afpor;
extern const uint32_t temp_afwdt;
extern const uint32_t temp_afoscsel;
extern const uint32_t temp_afsec;
#endif


#endif
