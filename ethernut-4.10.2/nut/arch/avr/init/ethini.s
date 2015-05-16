; to be included by crt*.s files
;
; Fake Ethernet 1.3 EEPROM
;

        ldi  R16, 0xC0
        out  0x35,R16

        ldi  R30, 0xE1
        sts  0x8300, R30
        ldi  R30, 0x40
        sts  0x8301, R30

        ldi  R16, 0xC0
        out  0x15, R16
        out  0x14, R16
        ldi  R16, 0x00
        out  0x35, R16

EmuLoop:
        ldi  R30, 0
        ldi  R31, 0
        sbis 0x13, 5
        rjmp EmuClkClr
EmuClkSet:
        adiw R30, 1
        breq EmuDone
        sbis 0x13, 5
        rjmp EmuLoop
        rjmp EmuClkSet
EmuClkClr:
        adiw R30, 1
        breq EmuDone
        sbic 0x13, 5
        rjmp EmuLoop
        rjmp EmuClkClr
EmuDone:
        out  0x15, R31
        out  0x14, R31
