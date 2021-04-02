#include <p32xxxx.h>
#include <inttypes.h>
#include <configBits.h>

/*
// Configuration bits for the PIC32MM0256GPM064 or 036? chip. PRIMARY configuration
const uint32_t __attribute__((section (".SECTION_FDEVOPT"))) temp_fdevopt = 
	0x00003FE7
	| ((0xF0C5) << _FDEVOPT_USERID_POSITION)	// UserID is F0C5. NOTE! for some reason, it's declared as _17_ bits wide in the .h file. 		
												// Hence this hack, to have it aligned, since the definition shifts by 15, instead of 16...
	| (0b0 << _FDEVOPT_FVBUSIO_POSITION)	// VBUSON pin is controlled by port function, not USB
	| (0b0 << _FDEVOPT_FUSBIDIO_POSITION)	// USBID pin is controlled by port function, not USB	
	| (0b1 << _FDEVOPT_ALTI2C_POSITION)	// SDA1 and SCL1 are on pins RB8 and RB9											
	| (0b1 << _FDEVOPT_SOSCHP_POSITION);	// SOSC operated in Normal Power mode

const uint32_t __attribute__((section (".SECTION_FICD"))) temp_ficd =
	0xFFFFFFE3
	| (0b10 << _FICD_ICS_POSITION)			// Communication is on PGEC2/PGED2
	| (0b1 << _FICD_JTAGEN_POSITION);		// JTAG is enabled

const uint32_t __attribute__((section (".SECTION_FPOR"))) temp_fpor =
	0xFFFFFFF0
	| (0b0 << _FPOR_LPBOREN_POSITION)		// Low-Power BOR is disabled
	| (0b0 << _FPOR_RETVR_POSITION)			// Retention regulator is enabled, controlled by RETEN in sleep
	| (0b00 << _FPOR_BOREN_POSITION);		// Brown-out reset disabled in HW, SBOREN bit disabled

const uint32_t __attribute__((section (".SECTION_FWDT"))) temp_fwdt =
	0xFFFF0000
	| (0b0 << _FWDT_FWDTEN_POSITION)		// Watchdog Timer disabled
	| (0b00 << _FWDT_RCLKSEL_POSITION)		// Clock source is system clock
	| (0b10000 << _FWDT_RWDTPS_POSITION)	// Run mode Watchdog Postscale is 1:65536
	| (0b1 << _FWDT_WINDIS_POSITION)		// Windowed mode is disabled
	| (0b00 << _FWDT_FWDTWINSZ_POSITION)	// Watchdog Timer window size is 75%
	| (0b10000 << _FWDT_SWDTPS_POSITION);	// Sleep mode Watchdog Postscale is 1:65536 

const uint32_t __attribute__((section (".SECTION_FOSCSEL"))) temp_foscsel =
	0xFFFF2828
	| (0b01 << _FOSCSEL_FCKSM_POSITION)		// Clock switching ENABLED, Fail-Safe clock monitor disabled
	| (0b1 << _FOSCSEL_SOSCSEL_POSITION)	// Crystal is used for SOSC, RA4/RB4 controlled by SOSC
	| (0b1 << _FOSCSEL_OSCIOFNC_POSITION)	// System clock not on CLKO pin, operates as normal I/O
	| (0b11 << _FOSCSEL_POSCMOD_POSITION)	// Primary oscillator is disabled
	| (0b0 << _FOSCSEL_IESO_POSITION)		// Two-speed Start-up is disabled
	| (0b0 << _FOSCSEL_SOSCEN_POSITION)		// Secondary oscillator disabled
	| (0b1 << _FOSCSEL_PLLSRC_POSITION)		// FRC is input to PLL on device Reset
	| (0b000 << _FOSCSEL_FNOSC_POSITION);	// Oscillator is FRC with "Divide-by-N". We will switch to PLL later.

const uint32_t __attribute__((section (".SECTION_FSEC"))) temp_fsec =
	0x7FFFFFFF
	| (0b1 << _FSEC_CP_POSITION);			// Code protection disabled
// End PRIMARY configuration

// Configuration bits for the PIC32MM chip. ALTERNATE configuration
const uint32_t __attribute__((section (".SECTION_AFDEVOPT"))) temp_afdevopt = 
	0x0000FFF7
	| ((0xF0C5) << _FDEVOPT_USERID_POSITION)	// UserID is F0C5. NOTE! for some reason, it's declared as _17_ bits wide in the .h file. 
												// Hence this hack, to have it aligned, since the definition shifts by 15, instead of 16...
	| (0b1 << _FDEVOPT_SOSCHP_POSITION);	// SOSC operated in Normal Power mode

const uint32_t __attribute__((section (".SECTION_AFICD"))) temp_aficd =
	0xFFFFFFE3
	| (0b01 << _FICD_ICS_POSITION)			// Communication is on PGEC3/PGED3
	| (0b1 << _FICD_JTAGEN_POSITION);		// JTAG is enabled

const uint32_t __attribute__((section (".SECTION_AFPOR"))) temp_afpor =
	0xFFFFFFF0
	| (0b0 << _FPOR_LPBOREN_POSITION)		// Low-Power BOR is disabled
	| (0b0 << _FPOR_RETVR_POSITION)			// Retention regulator is enabled, controlled by RETEN in sleep
	| (0b00 << _FPOR_BOREN_POSITION);		// Brown-out reset disabled in HW, SBOREN bit disabled

const uint32_t __attribute__((section (".SECTION_AFWDT"))) temp_afwdt =
	0xFFFF0000
	| (0b0 << _FWDT_FWDTEN_POSITION)		// Watchdog Timer disabled
	| (0b00 << _FWDT_RCLKSEL_POSITION)		// Clock source is system clock
	| (0b10000 << _FWDT_RWDTPS_POSITION)	// Run mode Watchdog Postscale is 1:65536
	| (0b1 << _FWDT_WINDIS_POSITION)		// Windowed mode is disabled
	| (0b00 << _FWDT_FWDTWINSZ_POSITION)	// Watchdog Timer window size is 75%
	| (0b10000 << _FWDT_SWDTPS_POSITION);	// Sleep mode Watchdog Postscale is 1:65536 

const uint32_t __attribute__((section (".SECTION_AFOSCSEL"))) temp_afoscsel =
	0xFFFF2828
	| (0b01 << _FOSCSEL_FCKSM_POSITION)		// Clock switching ENABLED, Fail-Safe clock monitor disabled
	| (0b1 << _FOSCSEL_SOSCSEL_POSITION)	// Crystal is used for SOSC, RA4/RB4 controlled by SOSC
	| (0b1 << _FOSCSEL_OSCIOFNC_POSITION)	// System clock not on CLKO pin, operates as normal I/O
	| (0b11 << _FOSCSEL_POSCMOD_POSITION)	// Primary oscillator is disabled
	| (0b0 << _FOSCSEL_IESO_POSITION)		// Two-speed Start-up is disabled
	| (0b0 << _FOSCSEL_SOSCEN_POSITION)		// Secondary oscillator disabled
	| (0b1 << _FOSCSEL_PLLSRC_POSITION)		// FRC is input to PLL on device Reset
	| (0b001 << _FOSCSEL_FNOSC_POSITION);	// Oscillator is FRC with "Divide-by-N". We will switch to PLL later.

const uint32_t __attribute__((section (".SECTION_AFSEC"))) temp_afsec =
	0x7FFFFFFF
	| (0b1 << _FSEC_CP_POSITION);			// Code protection disabled
// End ALTERNATE configuration
*/

// Configuration bits for the PIC32MM0064GPL038 chip. PRIMARY configuration
const uint32_t __attribute__((section (".SECTION_FDEVOPT"))) temp_fdevopt = 
	0x0000FFF7
	| ((0x8383 << 1) << _FDEVOPT_USERID_POSITION)	// UserID. Same <<1 thing as for some other devices...	
	| (0b1 << _FDEVOPT_SOSCHP_POSITION);		// SOSC operated in Normal Power mode

const uint32_t __attribute__((section (".SECTION_FICD"))) temp_ficd =
	0xFFFFFFE3
	| (0b10 << _FICD_ICS_POSITION)			// Communication is on PGEC2/PGED2
	| (0b0 << _FICD_JTAGEN_POSITION);		// JTAG is DISABLED

const uint32_t __attribute__((section (".SECTION_FPOR"))) temp_fpor =
	0xFFFFFFF0
	| (0b0 << _FPOR_LPBOREN_POSITION)		// Low-Power BOR is disabled
	| (0b0 << _FPOR_RETVR_POSITION)			// Retention regulator is enabled, controlled by RETEN in sleep
	| (0b00 << _FPOR_BOREN_POSITION);		// Brown-out reset disabled in HW, SBOREN bit disabled

const uint32_t __attribute__((section (".SECTION_FWDT"))) temp_fwdt =
	0xFFFF0000
	| (0b0 << _FWDT_FWDTEN_POSITION)		// Watchdog Timer disabled
	| (0b00 << _FWDT_RCLKSEL_POSITION)		// Clock source is system clock
	| (0b10000 << _FWDT_RWDTPS_POSITION)	// Run mode Watchdog Postscale is 1:65536
	| (0b1 << _FWDT_WINDIS_POSITION)		// Windowed mode is disabled
	| (0b00 << _FWDT_FWDTWINSZ_POSITION)	// Watchdog Timer window size is 75%
	| (0b10000 << _FWDT_SWDTPS_POSITION);	// Sleep mode Watchdog Postscale is 1:65536 

const uint32_t __attribute__((section (".SECTION_FOSCSEL"))) temp_foscsel =
	0xFFFF2828
	| (0b01 << _FOSCSEL_FCKSM_POSITION)		// Clock switching ENABLED, Fail-Safe clock monitor disabled - needed for switch to different clock speed
	| (0b0 << _FOSCSEL_SOSCSEL_POSITION)	// External clock connected to SOSCO pin (RA4 RB4 controlled by I/O PORTx)
	| (0b1 << _FOSCSEL_OSCIOFNC_POSITION)	// System clock not on CLKO pin, operates as normal I/O
	| (0b11 << _FOSCSEL_POSCMOD_POSITION)	// Primary oscillator is disabled
	| (0b0 << _FOSCSEL_IESO_POSITION)		// Two-speed Start-up is disabled
	| (0b0 << _FOSCSEL_SOSCEN_POSITION)		// Secondary oscillator disabled
	| (0b1 << _FOSCSEL_PLLSRC_POSITION)		// FRC is input to PLL on device Reset
	| (0b000 << _FOSCSEL_FNOSC_POSITION);	// Oscillator is FRC with "Divide-by-N". We will switch to PLL later.

const uint32_t __attribute__((section (".SECTION_FSEC"))) temp_fsec =
	0x7FFFFFFF
	| (0b1 << _FSEC_CP_POSITION);			// Code protection disabled
	
const uint32_t __attribute__((section (".SECTION_FRESERVED"))) temp_freserved =
	0x7FFFFFFF;
	
// End PRIMARY configuration

// Configuration bits for the PIC32MM chip. ALTERNATE configuration
const uint32_t __attribute__((section (".SECTION_AFDEVOPT"))) temp_afdevopt = 
	0x0000FFF7
	| ((0x5F51 << 1) << _FDEVOPT_USERID_POSITION)	// UserID. Same <<1 thing as for some other devices...
	| (0b1 << _FDEVOPT_SOSCHP_POSITION);		// SOSC operated in Normal Power mode

const uint32_t __attribute__((section (".SECTION_AFICD"))) temp_aficd =
	0xFFFFFFE3
	| (0b11 << _FICD_ICS_POSITION)			// Communication is on PGEC3/PGED3
	| (0b0 << _FICD_JTAGEN_POSITION);		// JTAG is DISABLED

const uint32_t __attribute__((section (".SECTION_AFPOR"))) temp_afpor =
	0xFFFFFFF0
	| (0b0 << _FPOR_LPBOREN_POSITION)		// Low-Power BOR is disabled
	| (0b0 << _FPOR_RETVR_POSITION)			// Retention regulator is enabled, controlled by RETEN in sleep
	| (0b00 << _FPOR_BOREN_POSITION);		// Brown-out reset disabled in HW, SBOREN bit disabled

const uint32_t __attribute__((section (".SECTION_AFWDT"))) temp_afwdt =
	0xFFFF0000
	| (0b0 << _FWDT_FWDTEN_POSITION)		// Watchdog Timer disabled
	| (0b00 << _FWDT_RCLKSEL_POSITION)		// Clock source is system clock
	| (0b10000 << _FWDT_RWDTPS_POSITION)	// Run mode Watchdog Postscale is 1:65536
	| (0b1 << _FWDT_WINDIS_POSITION)		// Windowed mode is disabled
	| (0b00 << _FWDT_FWDTWINSZ_POSITION)	// Watchdog Timer window size is 75%
	| (0b10000 << _FWDT_SWDTPS_POSITION);	// Sleep mode Watchdog Postscale is 1:65536 

const uint32_t __attribute__((section (".SECTION_AFOSCSEL"))) temp_afoscsel =
	0xFFFF2828
	| (0b01 << _FOSCSEL_FCKSM_POSITION)		// Clock switching ENABLED, Fail-Safe clock monitor disabled - needed for switch to different clock speed
	| (0b0 << _FOSCSEL_SOSCSEL_POSITION)	// External clock connected to SOSCO pin (RA4 RB4 controlled by I/O PORTx)
	| (0b1 << _FOSCSEL_OSCIOFNC_POSITION)	// System clock not on CLKO pin, operates as normal I/O
	| (0b11 << _FOSCSEL_POSCMOD_POSITION)	// Primary oscillator is disabled
	| (0b0 << _FOSCSEL_IESO_POSITION)		// Two-speed Start-up is disabled
	| (0b0 << _FOSCSEL_SOSCEN_POSITION)		// Secondary oscillator disabled
	| (0b1 << _FOSCSEL_PLLSRC_POSITION)		// FRC is input to PLL on device Reset
	| (0b000 << _FOSCSEL_FNOSC_POSITION);	// Oscillator is FRC with "Divide-by-N". We will switch to PLL later.

const uint32_t __attribute__((section (".SECTION_AFSEC"))) temp_afsec =
	0x7FFFFFFF
	| (0b1 << _FSEC_CP_POSITION);			// Code protection disabled
	
const uint32_t __attribute__((section (".SECTION_AFRESERVED"))) temp_afreserved =
	0x7FFFFFFF;
// End ALTERNATE configuration


