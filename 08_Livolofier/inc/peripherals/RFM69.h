#ifndef RFM69_H_f28ec970804a43cf88275d9e963fa9fb
#define RFM69_H_f28ec970804a43cf88275d9e963fa9fb

#include <inttypes.h>
#include <stdbool.h>

void RFM69_setReset(bool value);
void RFM69_SetReg(uint8_t reg, uint8_t val);
uint8_t RFM69_ReadReg(uint8_t reg);

void RFM69_Init();

void RFM69_GetRevision(uint8_t * outBuf);
uint8_t RFM69_GetTemp();
void RFM69_SetBitrate(uint32_t bitrate);
void RFM69_SetFreqDeviation(uint32_t deviation);
void RFM69_SetRfCarrierFreq(uint32_t centerFreq);
void RFM69_SetOutputPower(int32_t power);
void RFM69_SetOverCurrentProtection(bool enabled, uint8_t current);
void RFM69_SetLNA(bool impedance, uint8_t gain);
void RFM69_SetRxBw(bool modulation, uint8_t dccFreq, uint32_t rxBw);
int16_t RFM69_GetRSSI();
void RFM69_SetDioMapping(uint8_t dio, uint8_t map);
void RFM69_SetCLKOUT(uint8_t val);


#define RFM69_FXOSC	32000000	// 32MHz crystal on board
#define RFM69_FSTEP ((float)RFM69_FXOSC / 524288.0f)	// FXOSC / 2^19
// FSTEP needs to be a float!
// -> Datasheet says 61.0Hz, but FXOSC/2^19 is 61.03515625Hz
// -> 61.0Hz / 61.03515625Hz = 0.999424, (1-0.999424) * 100 = 0.0576% ERROR. THIS IS NOT INSIGNIFICANT!
// -> The error for example shows a 433.920MHz device at 433.670. Exactly the error rate away.

// Register name definitions taken from https://www.hoperf.com/data/upload/portal/20190307/RFM69HCW-V1.1.pdf

#define RFM69_RegFifo			0x00	// FIFO read/write access
#define RFM69_RegOpMode			0x01	// Operating modes of the transceiver
#define RFM69_RegDataModul		0x02	// Data operation mode and Modulation settings
#define RFM69_RegBitrateMsb		0x03	// Bit Rate setting, Most Significant Bits
#define RFM69_RegBitrateLsb		0x04	// Bit Rate setting, Least Significant Bits
#define RFM69_RegFdevMsb		0x05	// Frequency deviation setting, Most Significant Bits
#define RFM69_RegFdevLsb		0x06	// Frequency deviation setting, Least Significant Bits
#define RFM69_RegFrfMsb			0x07	// RF Carrier frequency, Most Significant Bits
#define RFM69_RegFrfMid			0x08	// RF Carrier frequency, Intermediate Bits
#define RFM69_RegFrfLsb			0x09	// RF Carrier frequency, Least Significant Bits
#define RFM69_RegOsc1			0x0A	// RC Oscillators Settings
#define RFM69_RegAfcCtrl		0x0B	// AFC control in low-modulation index situations
#define RFM69_RegReserved0C		0x0C	// -
#define RFM69_RegRegListen1		0x0D	// Listen Mode settings
#define RFM69_RegRegListen2		0x0E	// Listen Mode Idle duration
#define RFM69_RegListen3		0x0F	// Listen Mode RX duration
#define RFM69_RegVersion		0x10	// Version code of the chip Full rev + mask rev
#define RFM69_RegPaLevel		0x11	// PA selection and Output Power control
#define RFM69_RegPaRamp			0x12	// Control of the PA ramp time in FSK modes
#define RFM69_RegOcp			0x13	// Overcurrent Protection control
#define RFM69_RegReserved14		0x14	// -
#define RFM69_RegReserved15		0x15	// -
#define RFM69_RegReserved16		0x16	// -
#define RFM69_RegReserved17		0x17	// -
#define RFM69_RegLna			0x18	// Low Noise Amplifier settings
#define RFM69_RegRxBw			0x19	// Channel Filter BE Control
#define RFM69_RegAfcBw			0x1A	// Channel Filter BW control during the AFC routine
#define RFM69_RegOokPeak		0x1B	// OOK demodulator selection and control in peak mode
#define RFM69_RegOokAvg			0x1C	// Average threshold control of the OOK demodulator
#define RFM69_RegOokFix			0x1D	// Fixed threshold control of the OOK demodulator
#define RFM69_RegAfcFei			0x1E	// AFC and FEI control and the status
#define RFM69_RegAfcMsb			0x1F	// MSB of the frequency correction of the AFC
#define RFM69_RegAfcLsb			0x20	// LSB of the frequency correction of the AFC
#define RFM69_RegFeiMsb			0x21	// MSB of the calculated frequency error
#define RFM69_RegFeiLsb			0x22	// LSB of the calculated frequency error
#define RFM69_RegRssiConfig		0x23	// RSSI-related settings
#define RFM69_RegRssiValue		0x24	// RSSI value in dBm
#define RFM69_RegDioMapping1	0x25	// Mapping of pins DIO0 to DIO3
#define RFM69_RegDioMapping2	0x26	// Mapping of DIO4, DIO5, and ClkOut frequency
#define RFM69_RegIrqFlags1		0x27	// Status register - PLL Lock, Timeout, RSSI > Threshold...
#define RFM69_RegIrqFlags2		0x28	// Status register - FIFO handling flags
#define RFM69_RegRssiThresh		0x29	// RSSI Threshold control
#define RFM69_RegRxTimeout1		0x2A	// Timeout duration between RX request and RSSI detection
#define RFM69_RegTxTimeout2		0x2B	// Timeout duration between RSSI detection and PayloadReady
#define RFM69_RegPreambleMsb	0x2C	// Preamble length, MSB
#define RFM69_RegPreambleLsb	0x2D	// Preamble length, LSB
#define RFM69_RegSyncConfig		0x2E	// Sync Word Recognition control
#define RFM69_RegSyncValue1		0x2F	// Sync word, byte 1
#define RFM69_RegSyncValue2		0x30	// Sync word, byte 2
#define RFM69_RegSyncValue3		0x31	// Sync word, byte 3
#define RFM69_RegSyncValue4		0x32	// Sync word, byte 4
#define RFM69_RegSyncValue5		0x33	// Sync word, byte 5
#define RFM69_RegSyncValue6		0x34	// Sync word, byte 6
#define RFM69_RegSyncValue7		0x35	// Sync word, byte 7
#define RFM69_RegSyncValue8		0x36	// Sync word, byte 8
#define RFM69_RegPacketConfig1	0x37	// Packet mode settings
#define RFM69_RegPayloadLength	0x38	// Payload length settings
#define RFM69_RegNodeAdrs		0x39	// Node address
#define RFM69_RegBroadcastAdrs	0x3A	// Broadcast address
#define RFM69_RegAutoModes		0x3B	// Auto modes settings
#define RFM69_RegFifoThresh		0x3C	// FIFO threshold, TX start condition
#define RFM69_RegPacketConfig2	0x3D	// Packet mode settings
#define RFM69_RegAesKey1		0x3E	// Cypher key, byte 1
#define RFM69_RegAesKey2		0x3F	// Cypher key, byte 2
#define RFM69_RegAesKey3		0x40	// Cypher key, byte 3
#define RFM69_RegAesKey4		0x41	// Cypher key, byte 4
#define RFM69_RegAesKey5		0x42	// Cypher key, byte 5
#define RFM69_RegAesKey6		0x43	// Cypher key, byte 6
#define RFM69_RegAesKey7		0x44	// Cypher key, byte 7
#define RFM69_RegAesKey8		0x45	// Cypher key, byte 8
#define RFM69_RegAesKey9		0x46	// Cypher key, byte 9
#define RFM69_RegAesKey10		0x47	// Cypher key, byte 10
#define RFM69_RegAesKey11		0x48	// Cypher key, byte 11
#define RFM69_RegAesKey12		0x49	// Cypher key, byte 12
#define RFM69_RegAesKey13		0x4A	// Cypher key, byte 13
#define RFM69_RegAesKey14		0x4B	// Cypher key, byte 14
#define RFM69_RegAesKey15		0x4C	// Cypher key, byte 15
#define RFM69_RegAesKey16		0x4D	// Cypher key, byte 16
#define RFM69_RegTemp1			0x4E	// Temperature sensor control
#define RFM69_RegTemp2			0x4F	// Temperature readout
#define RFM69_RegTestLna		0x58	// Sensitivity boost
#define RFM69_RegTestPa1		0x5A	// High Power PA settings
#define RFM69_RegTestPa2		0x5C	// High Power PA settings
#define RFM69_RegTestDagc		0x6F	// Fading Margin Improvements
#define RFM69_RegTestAfc		0x71	// AFC offset for low modulation index AFC
// All regisers from 0x50 onward are internal test registers


// Bit values. Shift values (first bit pos) and Mask values provided

// COMMON CONFIGURATION REGISTERS

// RegFifo - whole reg is just FIFO data input/output

// RegOpMode bits
#define RFM69_RegOpMode_Bit_Shift_SequencerOff	(7)
#define RFM69_RegOpMode_Bit_Shift_ListenOn		(6)
#define RFM69_RegOpMode_Bit_Shift_ListenAbort	(5)
#define RFM69_RegOpMode_Bit_Shift_Mode			(2)
#define RFM69_RegOpMode_Bit_Mask_SequencerOff	(1 << RFM69_RegOpMode_Bit_Shift_SequencerOff)
#define RFM69_RegOpMode_Bit_Mask_ListenOn		(1 << RFM69_RegOpMode_Bit_Shift_ListenOn)
#define RFM69_RegOpMode_Bit_Mask_ListenAbort	(1 << RFM69_RegOpMode_Bit_Shift_ListenAbort)
#define RFM69_RegOpMode_Bit_Mask_Mode			(0b111 << RFM69_RegOpMode_Bit_Shift_Mode)

// RegDataModul bits
#define RFM69_RegDataModul_Bit_Shift_DataMode			(5)
#define RFM69_RegDataModul_Bit_Shift_ModulationType		(3)
#define RFM69_RegDataModul_Bit_Shift_ModulationShaping	(3)
#define RFM69_RegDataModul_Bit_Mask_DataMode			(0b11 << RFM69_RegDataModul_Bit_Shift_DataMode)
#define RFM69_RegDataModul_Bit_Mask_ModulationType		(0b11 << RFM69_RegDataModul_Bit_Shift_ModulationType)
#define RFM69_RegDataModul_Bit_Mask_ModulationShaping	(0b11 << RFM69_RegDataModul_Bit_Shift_ModulationShaping)

// RegBitrateMsb and RegBitrateLsb - both regs combine to form a 16-bit word

// RegFdevMsb and RegFdevLsb - both reg combine to form a 14-bit word (MSB in only 6-bits, right aligned!)

// RegFrfMsb, RegFrfMid, RegFrfLsb - all three combine into a 24-bit word

// RegOsc1 bits
#define RFM69_RegOsc1_Bit_Shift_RcCalStart			(7)
#define RFM69_RegOsc1_Bit_Shift_RcCalDone			(6)
#define RFM69_RegOsc1_Bit_Mask_RcCalStart			(1 << RFM69_RegOsc1_Bit_Shift_RcCalStart)
#define RFM69_RegOsc1_Bit_Mask_RcCalDone			(1 << RFM69_RegOsc1_Bit_Shift_RcCalDone)

// RegAdcCtrl bits
#define RFM69_RegAdcCtrl_Bit_Shift_AfcLowBetaOn		(5)
#define RFM69_RegAdcCtrl_Bit_Mask_AfcLowBetaOn		(1 << RFM69_RegAdcCtrl_Bit_Shift_AfcLowBetaOn)

// RegListen1 bits
#define RFM69_RegListen1_Bit_Shift_ListenReslIdle	(6)
#define RFM69_RegListen1_Bit_Shift_ListenReslRx		(4)
#define RFM69_RegListen1_Bit_Shift_ListenCriteria	(3)
#define RFM69_RegListen1_Bit_Shift_ListenEnd		(1)
#define RFM69_RegListen1_Bit_Mask_ListenReslIdle	(0b11 << RFM69_RegListen1_Bit_Shift_ListenReslIdle)
#define RFM69_RegListen1_Bit_Mask_ListenReslRx		(0b11 << RFM69_RegListen1_Bit_Shift_ListenReslRx)
#define RFM69_RegListen1_Bit_Mask_ListenCriteria	(1 << RFM69_RegListen1_Bit_Shift_ListenCriteria)
#define RFM69_RegListen1_Bit_Mask_ListenEnd			(0b11 << RFM69_RegListen1_Bit_Shift_ListenEnd)

// RegListen2 - whole reg is duration of Idle Phase in Listen mode

// RegListen3 - whole reg is duration of Rx Phase in Listen mode (startup time included)

// RegVersion bits
#define RFM69_RegVersion_Bit_Shift_FullRevNumber	(4)
#define RFM69_RegVersion_Bit_Shift_MetalMaskRev		(0)
#define RFM69_RegVersion_Bit_Mask_FullRevNumber		(0b1111 << RFM69_RegVersion_Bit_Shift_FullRevNumber)
#define RFM69_RegVersion_Bit_Mask_MetalMaskRev		(0b1111 << RFM69_RegVersion_Bit_Shift_MetalMaskRev)


// TRANSMITTER REGISTERS

// RegPaLevel bits
#define RFM69_RegPaLevel_Bit_Shift_Pa0On		(7)
#define RFM69_RegPaLevel_Bit_Shift_Pa1On		(6)
#define RFM69_RegPaLevel_Bit_Shift_Pa2On		(5)
#define RFM69_RegPaLevel_Bit_Shift_OutputPower	(0)
#define RFM69_RegPaLevel_Bit_Mask_Pa0On			(1 << RFM69_RegPaLevel_Bit_Shift_Pa0On)
#define RFM69_RegPaLevel_Bit_Mask_Pa1On			(1 << RFM69_RegPaLevel_Bit_Shift_Pa1On)
#define RFM69_RegPaLevel_Bit_Mask_Pa2On			(1 << RFM69_RegPaLevel_Bit_Shift_Pa2On)
#define RFM69_RegPaLevel_Bit_Mask_OutputPower	(0b11111 << RFM69_RegPaLevel_Bit_Shift_OutputPower)

// RegPaRamp bits
#define RFM69_RegPaRamp_Bit_Shift_PaRamp		(0)
#define RFM69_RegPaRamp_Bit_Mask_PaRamp			(0b1111 << RFM69_RegPaRamp_Bit_Shift_PaRamp)

// RegOcp bits
#define RFM69_RegOcp_Bit_Shift_OcpOn			(4)
#define RFM69_RegOcp_Bit_Shift_OcpTrim			(0)
#define RFM69_RegOcp_Bit_Mask_OcpOn				(1 << RFM69_RegOcp_Bit_Shift_OcpOn)
#define RFM69_RegOcp_Bit_Mask_OcpTrim			(0b1111 << RFM69_RegOcp_Bit_Mask_OcpTrim)


// RECEIVER SETTINGS

// RegLna bits
#define RFM69_RegLna_Bit_Shift_LnaZin			(7)
#define RFM69_RegLna_Bit_Shift_LnaCurrentGain	(3)
#define RFM69_RegLna_Bit_Shift_LnaGainSelect	(0)
#define RFM69_RegLna_Bit_Mask_LnaZin			(1 << RFM69_RegLna_Bit_Shift_LnaZin)
#define RFM69_RegLna_Bit_Mask_LnaCurrentGain	(0b111 << RFM69_RegLna_Bit_Shift_LnaCurrentGain)
#define RFM69_RegLna_Bit_Mask_LnaGainSelect		(0b111 << RFM69_RegLna_Bit_Shift_LnaGainSelect)

// RegRxBw bits
#define RFM69_RegRxBw_Bit_Shift_DccFreq			(5)
#define RFM69_RegRxBw_Bit_Shift_RxBwMant		(3)
#define RFM69_RegRxBw_Bit_Shift_RxBwExp			(0)
#define RFM69_RegRxBw_Bit_Mask_DccFreq			(0b111 << RFM69_RegRxBw_Bit_Shift_DccFreq)
#define RFM69_RegRxBw_Bit_Mask_RxBwMant			(0b11 << RFM69_RegRxBw_Bit_Shift_RxBwMant)
#define RFM69_RegRxBw_Bit_Mask_RxBwExp			(0b111 << RFM69_RegRxBw_Bit_Shift_RxBwExp)

// RegAfcBw bits
#define RFM69_RegAfcBw_Bit_Shift_DccFreqAfc		(5)
#define RFM69_RegAfcBw_Bit_Shift_RxBwMantAfc	(3)
#define RFM69_RegAfcBw_Bit_Shift_RxBwExpAfc		(0)
#define RFM69_RegAfcBw_Bit_Mask_DccFreqAfc		(0b111 << RFM69_RegAfcBw_Bit_Shift_DccFreq)
#define RFM69_RegAfcBw_Bit_Mask_RxBwMantAfc		(0b11 << RFM69_RegAfcBw_Bit_Shift_RxBwMant)
#define RFM69_RegAfcBw_Bit_Mask_RxBwExpAfc		(0b111 << RFM69_RegAfcBw_Bit_Shift_RxBwExp)

// RegOokPeak bits
#define RFM69_RegOokPeak_Bit_Shift_OokThreshType		(6)
#define RFM69_RegOokPeak_Bit_Shift_OokPeakThreshStep	(3)
#define RFM69_RegOokPeak_Bit_Shift_OokPeakThreshDec		(0)
#define RFM69_RegOokPeak_Bit_Mask_OokThreshType			(0b11 << RFM69_RegOokPeak_Bit_Shift_OokThreshType)
#define RFM69_RegOokPeak_Bit_Mask_OokPeakThreshStep		(0b111 << RFM69_RegOokPeak_Bit_Shift_OokPeakThreshStep)
#define RFM69_RegOokPeak_Bit_Mask_OokPeakThreshDec		(0b111 << RFM69_RegOokPeak_Bit_Shift_OokPeakThreshDec)

// RegOokAvg bits
#define RFM69_RegOokAvg_Bit_Shift_OokAverageThreshFilt	(6)
#define RFM69_RegOokAvg_Bit_Mask_OokAverageThreshFilt	(0b11 << RFM69_RegOokAvg_Bit_Shift_OokAverageThreshFilt)

// RegOokFix - Whole reg sets threshold in dB in the OOK demodulator, then OokThresType == 0b00

// RegAfcFei bits
#define RFM69_RegAfcFei_Bit_Shift_FeiDone			(6)
#define RFM69_RegAfcFei_Bit_Shift_FeiStart			(5)
#define RFM69_RegAfcFei_Bit_Shift_AfcDone			(4)
#define RFM69_RegAfcFei_Bit_Shift_AfcAutoclearOn	(3)
#define RFM69_RegAfcFei_Bit_Shift_AfcAutoOn			(2)
#define RFM69_RegAfcFei_Bit_Shift_AfcClear			(1)
#define RFM69_RegAfcFei_Bit_Shift_AfcStart			(0)
#define RFM69_RegAfcFei_Bit_Mask_FeiDone			(1 << RFM69_RegAfcFei_Bit_Shift_FeiDone)
#define RFM69_RegAfcFei_Bit_Mask_FeiStart			(1 << RFM69_RegAfcFei_Bit_Shift_FeiStart)
#define RFM69_RegAfcFei_Bit_Mask_AfcDone			(1 << RFM69_RegAfcFei_Bit_Shift_AfcDone)
#define RFM69_RegAfcFei_Bit_Mask_AfcAutoclearOn		(1 << RFM69_RegAfcFei_Bit_Shift_AfcAutoclearOn)
#define RFM69_RegAfcFei_Bit_Mask_AfcAutoOn			(1 << RFM69_RegAfcFei_Bit_Shift_AfcAutoOn)
#define RFM69_RegAfcFei_Bit_Mask_AfcClear			(1 << RFM69_RegAfcFei_Bit_Shift_AfcClear)
#define RFM69_RegAfcFei_Bit_Mask_AfcStart			(1 << RFM69_RegAfcFei_Bit_Shift_AfcStart)

// RegAfcMsb and RegAfcLsb - both reg used as 16-bit word for reading AfcValue. 2's complement!

// RegFeiMsb and RegFeiLsb - both reg used as 16-bit word for reading Measured frequency offset. 2's complement!

// RegRssiConfig bits
#define RFM69_RegRssiConfig_Bit_Shift_RssiDone		(1)
#define RFM69_RegRssiConfig_Bit_Shift_RssiStart		(0)
#define RFM69_RegRssiConfig_Bit_Mask_RssiDone		(1 << RFM69_RegRssiConfig_Bit_Shift_RssiDone)
#define RFM69_RegRssiConfig_Bit_Mask_RssiStart		(1 << RFM69_RegRssiConfig_Bit_Shift_RssiStart)

// RegRssiValue - whole register shows measured RSSI in dBm, in 0.5dB steps


// IRQ and PIN MAPPING REGISTERS

// RegDioMapping1 bits
#define RFM69_RegDioMapping1_Bit_Shift_Dio0Mapping	(6)
#define RFM69_RegDioMapping1_Bit_Shift_Dio1Mapping	(4)
#define RFM69_RegDioMapping1_Bit_Shift_Dio2Mapping	(2)
#define RFM69_RegDioMapping1_Bit_Shift_Dio3Mapping	(0)
#define RFM69_RegDioMapping1_Bit_Mask_Dio0Mapping	(0b11 << RFM69_RegDioMapping1_Bit_Shift_Dio0Mapping)
#define RFM69_RegDioMapping1_Bit_Mask_Dio1Mapping	(0b11 << RFM69_RegDioMapping1_Bit_Shift_Dio1Mapping)
#define RFM69_RegDioMapping1_Bit_Mask_Dio2Mapping	(0b11 << RFM69_RegDioMapping1_Bit_Shift_Dio2Mapping)
#define RFM69_RegDioMapping1_Bit_Mask_Dio3Mapping	(0b11 << RFM69_RegDioMapping1_Bit_Shift_Dio3Mapping)

// RegDioMapping2 bits
#define RFM69_RegDioMapping2_Bit_Shift_Dio4Mapping	(6)
#define RFM69_RegDioMapping2_Bit_Shift_Dio5Mapping	(4)
#define RFM69_RegDioMapping2_Bit_Shift_ClkOut		(0)
#define RFM69_RegDioMapping2_Bit_Mask_Dio4Mapping	(0b11 << RFM69_RegDioMapping2_Bit_Shift_Dio4Mapping)
#define RFM69_RegDioMapping2_Bit_Mask_Dio5Mapping	(0b11 << RFM69_RegDioMapping2_Bit_Shift_Dio5Mapping)
#define RFM69_RegDioMapping2_Bit_Mask_ClkOut		(0b111 << RFM69_RegDioMapping2_Bit_Shift_ClkOut)

// RegIrqFlags1 bits
#define RFM69_RegIrqFlags1_Bit_Shift_ModeReady			(7)
#define RFM69_RegIrqFlags1_Bit_Shift_RxReady			(6)
#define RFM69_RegIrqFlags1_Bit_Shift_TxReady			(5)
#define RFM69_RegIrqFlags1_Bit_Shift_PllLock			(4)
#define RFM69_RegIrqFlags1_Bit_Shift_Rssi				(3)
#define RFM69_RegIrqFlags1_Bit_Shift_Timeout			(2)
#define RFM69_RegIrqFlags1_Bit_Shift_AutoMode			(1)
#define RFM69_RegIrqFlags1_Bit_Shift_SyncAddressMatch	(0)
#define RFM69_RegIrqFlags1_Bit_Mask_ModeReady			(1 << RFM69_RegIrqFlags1_Bit_Shift_ModeReady)
#define RFM69_RegIrqFlags1_Bit_Mask_RxReady				(1 << RFM69_RegIrqFlags1_Bit_Shift_RxReady)
#define RFM69_RegIrqFlags1_Bit_Mask_TxReady				(1 << RFM69_RegIrqFlags1_Bit_Shift_TxReady)
#define RFM69_RegIrqFlags1_Bit_Mask_PllLock				(1 << RFM69_RegIrqFlags1_Bit_Shift_PllLock)
#define RFM69_RegIrqFlags1_Bit_Mask_Rssi				(1 << RFM69_RegIrqFlags1_Bit_Shift_Rssi)
#define RFM69_RegIrqFlags1_Bit_Mask_Timeout				(1 << RFM69_RegIrqFlags1_Bit_Shift_Timeout)
#define RFM69_RegIrqFlags1_Bit_Mask_AutoMode			(1 << RFM69_RegIrqFlags1_Bit_Shift_AutoMode)
#define RFM69_RegIrqFlags1_Bit_Mask_SyncAddressMatch	(1 << RFM69_RegIrqFlags1_Bit_Shift_SyncAddressMatch)

// RegIrqFlags2 bits
#define RFM69_RegIrqFlags2_Bit_Shift_FifoFull			(7)
#define RFM69_RegIrqFlags2_Bit_Shift_FifoNotEmpty		(6)
#define RFM69_RegIrqFlags2_Bit_Shift_FifoLevel			(5)
#define RFM69_RegIrqFlags2_Bit_Shift_FifoOverrun		(4)
#define RFM69_RegIrqFlags2_Bit_Shift_PacketSent			(3)
#define RFM69_RegIrqFlags2_Bit_Shift_PayloadReady		(2)
#define RFM69_RegIrqFlags2_Bit_Shift_CrcOk				(1)
#define RFM69_RegIrqFlags2_Bit_Mask_FifoFull			(1 << RFM69_RegIrqFlags2_Bit_Shift_FifoFull)
#define RFM69_RegIrqFlags2_Bit_Mask_FifoNotEmpty		(1 << RFM69_RegIrqFlags2_Bit_Shift_FifoNotEmpty)
#define RFM69_RegIrqFlags2_Bit_Mask_FifoLevel			(1 << RFM69_RegIrqFlags2_Bit_Shift_FifoLevel)
#define RFM69_RegIrqFlags2_Bit_Mask_FifoOverrun			(1 << RFM69_RegIrqFlags2_Bit_Shift_FifoOverrun)
#define RFM69_RegIrqFlags2_Bit_Mask_PacketSent			(1 << RFM69_RegIrqFlags2_Bit_Shift_PacketSent)
#define RFM69_RegIrqFlags2_Bit_Mask_PayloadReady		(1 << RFM69_RegIrqFlags2_Bit_Shift_PayloadReady)
#define RFM69_RegIrqFlags2_Bit_Mask_CrcOk				(1 << RFM69_RegIrqFlags2_Bit_Shift_CrcOk)

// RegRssiThresh - whole reg sets the RSSI trigger level for the RSSI interrupt, 0.5db/bit?

// RegRxTimeout1 - whole reg sets time for interrupt, when swicthing to Rx mode and RSSI interrupt doesn't occur

// RegRxTimeout2 - whole reg sets time for interrupt, when RSSI interrupt occurs, but PayloadReady does not


// PACKET ENGINE REGISTERS

// RegPreambleMsb and RegPreambleLsb - both regs combine into a 16-bit word, size of preamble to be sent on TxStartCondition met

// RegSyncConfig bits
#define RFM69_RegSyncConfig_Bit_Shift_SyncOn			(7)
#define RFM69_RegSyncConfig_Bit_Shift_FifoFillCondition	(6)
#define RFM69_RegSyncConfig_Bit_Shift_SyncSize			(3)
#define RFM69_RegSyncConfig_Bit_Shift_SyncTol			(0)
#define RFM69_RegSyncConfig_Bit_Mask_SyncOn				(1 << RFM69_RegSyncConfig_Bit_Shift_SyncOn)
#define RFM69_RegSyncConfig_Bit_Mask_FifoFillCondition	(1 << RFM69_RegSyncConfig_Bit_Shift_FifoFillCondition)
#define RFM69_RegSyncConfig_Bit_Mask_SyncSize			(0b111 << RFM69_RegSyncConfig_Bit_Shift_SyncSize)
#define RFM69_RegSyncConfig_Bit_Mask_SyncTol			(0b111 << RFM69_RegSyncConfig_Bit_Shift_SyncTol)

// RegSyncValue1 -> RegSyncValue8 are 1st to 8th byte of Sync word to be send

// RegPacketConfig1 bits
#define RFM69_RegPacketConfig1_Bit_Shift_PacketFormat		(7)
#define RFM69_RegPacketConfig1_Bit_Shift_DcFree				(5)
#define RFM69_RegPacketConfig1_Bit_Shift_CrcOn				(4)
#define RFM69_RegPacketConfig1_Bit_Shift_CrcAutoClearOff	(3)
#define RFM69_RegPacketConfig1_Bit_Shift_AddressFiltering	(1)
#define RFM69_RegPacketConfig1_Bit_Mask_PacketFormat		(1 << RFM69_RegPacketConfig1_Bit_Shift_PacketFormat)
#define RFM69_RegPacketConfig1_Bit_Mask_DcFree				(0b11 << RFM69_RegPacketConfig1_Bit_Shift_DcFree)
#define RFM69_RegPacketConfig1_Bit_Mask_CrcOn				(1 << RFM69_RegPacketConfig1_Bit_Shift_CrcOn)
#define RFM69_RegPacketConfig1_Bit_Mask_CrcAutoClearOff		(1 << RFM69_RegPacketConfig1_Bit_Shift_CrcAutoClearOff)
#define RFM69_RegPacketConfig1_Bit_Mask_AddressFiltering	(0b11 << RFM69_RegPacketConfig1_Bit_Shift_AddressFiltering)

// RegPayloadLength - whole reg sets/gets payload length (fixed mode), or maximum length in Rx (variable mode)

// RegNodeAdrs - whole reg is Node address in address filtering

// RegBroadcastAdrs - whole reg is Broadcast address in address filtering

// RegAutoModes bits
#define RFM69_RegAutoModes_Bit_Shift_EnterCondition		(5)
#define RFM69_RegAutoModes_Bit_Shift_ExitCondition		(2)
#define RFM69_RegAutoModes_Bit_Shift_IntermediateMode	(0)
#define RFM69_RegAutoModes_Bit_Mode_EnterCondition		(0b111 << RFM69_RegAutoModes_Bit_Shift_EnterCondition)
#define RFM69_RegAutoModes_Bit_Mode_ExitCondition		(0b111 << RFM69_RegAutoModes_Bit_Shift_ExitCondition)
#define RFM69_RegAutoModes_Bit_Mode_IntermediateMode	(0b11 << RFM69_RegAutoModes_Bit_Shift_IntermediateMode)

// RegFifoThresh bits
#define RFM69_RegFifoThresh_Bit_Shift_TxStartCondition	(7)
#define RFM69_RegFifoThresh_Bit_Shift_FifoThreshold		(0)
#define RFM69_RegFifoThresh_Bit_Mask_TxStartCondition	(1 << RFM69_RegFifoThresh_Bit_Shift_TxStartCondition)
#define RFM69_RegFifoThresh_Bit_Mask_FifoThreshold		(0b1111111 << RFM69_RegFifoThresh_Bit_Shift_FifoThreshold)

// RegPacketConfig2 bits
#define RFM69_RegPacketConfig2_Bit_Shift_InterPacketRxDelay	(4)
#define RFM69_RegPacketConfig2_Bit_Shift_RestartRx			(2)
#define RFM69_RegPacketConfig2_Bit_Shift_AutoRxRestartOn	(1)
#define RFM69_RegPacketConfig2_Bit_Shift_AesOn				(0)
#define RFM69_RegPacketConfig2_Bit_Mask_InterPacketRxDelay	(0b1111 << RFM69_RegPacketConfig2_Bit_Shift_InterPacketRxDelay)
#define RFM69_RegPacketConfig2_Bit_Mask_RestartRx			(1 << RFM69_RegPacketConfig2_Bit_Shift_RestartRx)
#define RFM69_RegPacketConfig2_Bit_Mask_AutoRxRestartOn		(1 << RFM69_RegPacketConfig2_Bit_Shift_AutoRxRestartOn)
#define RFM69_RegPacketConfig2_Bit_Mask_AesOn				(1 << RFM69_RegPacketConfig2_Bit_Shift_AesOn)

// RegAesKey1 -> RegAesKey16 are 1st(MSB) to 16th (LSB) byte of the AES key 



// TEMPERATURE SENSOR REGISTERS

// RegTemp1 bits 
#define RFM69_RegTemp1_Bit_Shift_TempMeasureStart	(3)
#define RFM69_RegTemp1_Bit_Shift_TempMeasureRunning	(2)
#define RFM69_RegTemp1_Bit_Mask_TempMeasureStart	(1 << RFM69_RegTemp1_Bit_Shift_TempMeasureStart)	// Set to 1 to trigger conversion
#define RFM69_RegTemp1_Bit_Mask_TempMeasureRunning	(1 << RFM69_RegTemp1_Bit_Shift_TempMeasureRunning)	// Poll until 0

// RegTemp2 - whole register is measured temperature, -1°C/LSB, with 150 being "typ. room temperature"...


// TEST REGISTERS
// All are whole registers

#endif























