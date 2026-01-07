//============
// firmware.S
//============

.section ".data"


//===============
// Configuration
//===============

.globl wifi_config
.globl wifi_config_size

.align 4
wifi_config:
.incbin "Firmware/brcmfmac43455-sdio.txt"
.align 4
wifi_config_end:
.word 0
.word 0

wifi_config_size:
.word wifi_config_end-wifi_config


//==========
// Firmware
//==========

.globl wifi_firmware
.globl wifi_firmware_size

.align 4
wifi_firmware:
.incbin "Firmware/cyfmac43455-sdio-minimal.bin"
.align 4
wifi_firmware_end:

wifi_firmware_size:
.word wifi_firmware_end-wifi_firmware


//============
// Regulatory
//============

.globl wifi_clm
.globl wifi_clm_size

.align 4
wifi_clm:
.incbin "Firmware/cyfmac43455-sdio.clm_blob"
wifi_clm_end:

wifi_clm_size:
.word wifi_clm_end-wifi_clm
