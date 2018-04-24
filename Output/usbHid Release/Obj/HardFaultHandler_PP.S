# 1 "C:\\projects\\segger\\system\\HardFaultHandler.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "C:\\projects\\segger\\system\\HardFaultHandler.S"
;
# 28 "C:\\projects\\segger\\system\\HardFaultHandler.S"
;
# 47 "C:\\projects\\segger\\system\\HardFaultHandler.S"
;
# 65 "C:\\projects\\segger\\system\\HardFaultHandler.S"
        .global HardFault_Handler
        .type HardFault_Handler, function



;
# 87 "C:\\projects\\segger\\system\\HardFaultHandler.S"
        .extern HardFaultHandler



;
# 100 "C:\\projects\\segger\\system\\HardFaultHandler.S"
        .syntax unified
        .thumb
        .balign 4
        .text







;






;
# 130 "C:\\projects\\segger\\system\\HardFaultHandler.S"
HardFault_Handler:
# 159 "C:\\projects\\segger\\system\\HardFaultHandler.S"
        ;
        tst LR, #4 ;
        ite EQ
        mrseq R0, MSP ;
        mrsne R0, PSP ;
        b HardFaultHandler ;



        .end




;
