// Simple GNU arm assembly startup file for ARM Cortex M4 Tiva
// norbert.balbierer@oth-regensburg.de

	.syntax unified
	.arch armv7e-m

// Stack set-up
	.section .stack
	.align 3
	.equ Stack_Size, 0x00000400
	.global __StackTop
	.global __StackLimit

__StackLimit:
	.space Stack_Size
	.size __StackLimit, .-__StackLimit

__StackTop:
	.size __StackTop, .-__StackTop

// Reset handler
	.section .text
	.thumb_func
	.align 2
	.global Reset_Handler
	.type Reset_Handler, %function

Reset_Handler:
	// Set initial stack pointer
	ldr	r0, =__stack
	mov	sp, r0	
	// Copy .data segment from flash ro SRAM
	ldr	r0, =_start_data
	ldr	r1, =_end_data
	ldr	r2, =_end_text
	movs	r3, #0
	b	loop_copydata

copydata:
	ldr	r4, [r2, r3]
	str	r4, [r0, r3]
	// Increment R3 by 4 (Next Word Address)
	adds	r3, r3, #4 


loop_copydata:
	// R4 holds current address in .data and
	// is incremented with each step. 
	// (Base address _data + R3)
	adds	r4, r0, r3
	// When R4 has reached _edata, we're done.
	cmp 	r4, r1
	bcc	copydata

	// Branch to user code
	bl	main

	.pool
	.size Reset_Handler, .-Reset_Handler

// Default interrupt handler
	.align 1
	.thumb_func
	.weak Default_Handler
	.type Default_Handler, %function

Default_Handler:
	b	.

	.size Default_Handler, .-Default_Handler

// Vector table
	.section .vectors
	.align 2
	.global __Vectors

__Vectors:
    .word    __stack                            // Top of Stack
    .word    Reset_Handler                      // Reset Handler
    .word    NMI_Handler                        // NMI Handler
    .word    HardFault_Handler                  // Hard Fault Handler
    .word    MemManage_Handler                  // MPU Fault Handler
    .word    BusFault_Handler                   // Bus Fault Handler
    .word    UsageFault_Handler                 // Usage Fault Handler
    .word    0                                  // Reserved
    .word    0                                  // Reserved
    .word    0                                  // Reserved
    .word    0                                  // Reserved
    .word    SVC_Handler                        // SVCall Handler
    .word    DebugMon_Handler                   // Debug Monitor Handler
    .word    0                                  // Reserved
    .word    PendSV_Handler                     // PendSV Handler
    .word    SysTick_Handler                    // SysTick Handler
    
    // External Interrupts
    .word    GPIOPortA_Handler                  //   0: GPIO Port A
    .word    GPIOPortB_Handler                  //   1: GPIO Port B
    .word    GPIOPortC_Handler                  //   2: GPIO Port C
    .word    GPIOPortD_Handler                  //   3: GPIO Port D
    .word    GPIOPortE_Handler                  //   4: GPIO Port E
    .word    UART0_Handler                      //   5: UART0 Rx and Tx
    .word    UART1_Handler                      //   6: UART1 Rx and Tx
    .word    SSI0_Handler                       //   7: SSI0 Rx and Tx
    .word    I2C0_Handler                       //   8: I2C0 Master and Slave
    .word    PWM0Fault_Handler                  //   9: PWM Fault
    .word    PWM0Generator0_Handler             //  10: PWM Generator 0
    .word    PWM0Generator1_Handler             //  11: PWM Generator 1
    .word    PWM0Generator2_Handler             //  12: PWM Generator 2
    .word    Quadrature0_Handler                //  13: Quadrature Encoder 0
    .word    ADC0Seq0_Handler                   //  14: ADC Sequence 0
    .word    ADC0Seq1_Handler                   //  15: ADC Sequence 1
    .word    ADC0Seq2_Handler                   //  16: ADC Sequence 2
    .word    ADC0Seq3_Handler                   //  17: ADC Sequence 3
    .word    WDT_Handler                        //  18: Watchdog timer
    .word    Timer0A_Handler                    //  19: Timer 0 subtimer A
    .word    Timer0B_Handler                    //  20: Timer 0 subtimer B
    .word    Timer1A_Handler                    //  21: Timer 1 subtimer A
    .word    Timer1B_Handler                    //  22: Timer 1 subtimer B
    .word    Timer2A_Handler                    //  23: Timer 2 subtimer A
    .word    Timer2B_Handler                    //  24: Timer 2 subtimer B
    .word    Comp0_Handler                      //  25: Analog Comparator 0
    .word    Comp1_Handler                      //  26: Analog Comparator 1
    .word    Comp2_Handler                      //  27: Analog Comparator 2
    .word    SysCtl_Handler                     //  28: System Control (PLL, OSC, B
    .word    FlashCtl_Handler                   //  29: FLASH Control
    .word    GPIOPortF_Handler                  //  30: GPIO Port F
    .word    GPIOPortG_Handler                  //  31: GPIO Port G
    .word    GPIOPortH_Handler                  //  32: GPIO Port H
    .word    UART2_Handler                      //  33: UART2 Rx and Tx
    .word    SSI1_Handler                       //  34: SSI1 Rx and Tx
    .word    Timer3A_Handler                    //  35: Timer 3 subtimer A
    .word    Timer3B_Handler                    //  36: Timer 3 subtimer B
    .word    I2C1_Handler                       //  37: I2C1 Master and Slave
    .word    Quadrature1_Handler                //  38: Quadrature Encoder 1
    .word    CAN0_Handler                       //  39: CAN0
    .word    CAN1_Handler                       //  40: CAN1
    .word    CAN2_Handler                       //  41: CAN2
    .word    0                                  //  42: Reserved
    .word    Hibernate_Handler                  //  43: Hibernate
    .word    USB0_Handler                       //  44: USB0
    .word    PWM0Generator3_Handler             //  45: PWM Generator 3
    .word    uDMA_Handler                       //  46: uDMA Software Transfer
    .word    uDMA_Error                         //  47: uDMA Error
    .word    ADC1Seq0_Handler                   //  48: ADC1 Sequence 0
    .word    ADC1Seq1_Handler                   //  49: ADC1 Sequence 1
    .word    ADC1Seq2_Handler                   //  50: ADC1 Sequence 2
    .word    ADC1Seq3_Handler                   //  51: ADC1 Sequence 3
    .word    0                                  //  52: Reserved
    .word    0                                  //  53: Reserved
    .word    0                                  //  54: Reserved
    .word    0                                  //  55: Reserved
    .word    0                                  //  56: Reserved
    .word    SSI2_Handler                       //  57: SSI2 Rx and Tx
    .word    SSI3_Handler                       //  58: SSI3 Rx and Tx
    .word    UART3_Handler                      //  59: UART3 Rx and Tx
    .word    UART4_Handler                      //  60: UART4 Rx and Tx
    .word    UART5_Handler                      //  61: UART5 Rx and Tx
    .word    UART6_Handler                      //  62: UART6 Rx and Tx
    .word    UART7_Handler                      //  63: UART7 Rx and Tx
    .word    0                                  //  64: Reserved
    .word    0                                  //  65: Reserved
    .word    0                                  //  66: Reserved
    .word    0                                  //  67: Reserved
    .word    I2C2_Handler                       //  68: I2C2 Master and Slave
    .word    I2C3_Handler                       //  69: I2C3 Master and Slave
    .word    Timer4A_Handler                    //  70: Timer 4 subtimer A
    .word    Timer4B_Handler                    //  71: Timer 4 subtimer B
    .word    0                                  //  72: Reserved
    .word    0                                  //  73: Reserved
    .word    0                                  //  74: Reserved
    .word    0                                  //  75: Reserved
    .word    0                                  //  76: Reserved
    .word    0                                  //  77: Reserved
    .word    0                                  //  78: Reserved
    .word    0                                  //  79: Reserved
    .word    0                                  //  80: Reserved
    .word    0                                  //  81: Reserved
    .word    0                                  //  82: Reserved
    .word    0                                  //  83: Reserved
    .word    0                                  //  84: Reserved
    .word    0                                  //  85: Reserved
    .word    0                                  //  86: Reserved
    .word    0                                  //  87: Reserved
    .word    0                                  //  88: Reserved
    .word    0                                  //  89: Reserved
    .word    0                                  //  90: Reserved
    .word    0                                  //  91: Reserved
    .word    Timer5A_Handler                    //  92: Timer 5 subtimer A
    .word    Timer5B_Handler                    //  93: Timer 5 subtimer B
    .word    WideTimer0A_Handler                //  94: Wide Timer 0 subtimer A
    .word    WideTimer0B_Handler                //  95: Wide Timer 0 subtimer B
    .word    WideTimer1A_Handler                //  96: Wide Timer 1 subtimer A
    .word    WideTimer1B_Handler                //  97: Wide Timer 1 subtimer B
    .word    WideTimer2A_Handler                //  98: Wide Timer 2 subtimer A
    .word    WideTimer2B_Handler                //  99: Wide Timer 2 subtimer B
    .word    WideTimer3A_Handler                // 100: Wide Timer 3 subtimer A
    .word    WideTimer3B_Handler                // 101: Wide Timer 3 subtimer B
    .word    WideTimer4A_Handler                // 102: Wide Timer 4 subtimer A
    .word    WideTimer4B_Handler                // 103: Wide Timer 4 subtimer B
    .word    WideTimer5A_Handler                // 104: Wide Timer 5 subtimer A
    .word    WideTimer5B_Handler                // 105: Wide Timer 5 subtimer B
    .word    FPU_Handler                        // 106: FPU
    .word    0                                  // 107: Reserved
    .word    0                                  // 108: Reserved
    .word    I2C4_Handler                       // 109: I2C4 Master and Slave
    .word    I2C5_Handler                       // 110: I2C5 Master and Slave
    .word    GPIOPortM_Handler                  // 111: GPIO Port M
    .word    GPIOPortN_Handler                  // 112: GPIO Port N
    .word    Quadrature2_Handler                // 113: Quadrature Encoder 2
    .word    0                                  // 114: Reserved
    .word    0                                  // 115: Reserved
    .word    GPIOPortP_Handler                  // 116: GPIO Port P (Summary or P0)
    .word    GPIOPortP1_Handler                 // 117: GPIO Port P1
    .word    GPIOPortP2_Handler                 // 118: GPIO Port P2
    .word    GPIOPortP3_Handler                 // 119: GPIO Port P3
    .word    GPIOPortP4_Handler                 // 120: GPIO Port P4
    .word    GPIOPortP5_Handler                 // 121: GPIO Port P5
    .word    GPIOPortP6_Handler                 // 122: GPIO Port P6
    .word    GPIOPortP7_Handler                 // 123: GPIO Port P7
    .word    GPIOPortQ_Handler                  // 124: GPIO Port Q (Summary or Q0)
    .word    GPIOPortQ1_Handler                 // 125: GPIO Port Q1
    .word    GPIOPortQ2_Handler                 // 126: GPIO Port Q2
    .word    GPIOPortQ3_Handler                 // 127: GPIO Port Q3
    .word    GPIOPortQ4_Handler                 // 128: GPIO Port Q4
    .word    GPIOPortQ5_Handler                 // 129: GPIO Port Q5
    .word    GPIOPortQ6_Handler                 // 130: GPIO Port Q6
    .word    GPIOPortQ7_Handler                 // 131: GPIO Port Q7
    .word    GPIOPortR_Handler                  // 132: GPIO Port R
    .word    GPIOPortS_Handler                  // 133: GPIO Port S
    .word    PWM1Generator0_Handler             // 134: PWM 1 Generator 0
    .word    PWM1Generator1_Handler             // 135: PWM 1 Generator 1
    .word    PWM1Generator2_Handler             // 136: PWM 1 Generator 2
    .word    PWM1Generator3_Handler             // 137: PWM 1 Generator 3
    .word    PWM1Fault_Handler                  // 138: PWM 1 Fault

	.size __Vectors, .-__Vectors

	.macro def_irq_handler handler_name
	.weak \handler_name
	.set \handler_name, Default_Handler
	.endm

	def_irq_handler	NMI_Handler		// Default_Handler
	def_irq_handler	HardFault_Handler	// Default_Handler
	def_irq_handler	MemManage_Handler	// Default_Handler
	def_irq_handler	BusFault_Handler	// Default_Handler
	def_irq_handler	UsageFault_Handler	// Default_Handler
	def_irq_handler	SVC_Handler		// Default_Handler*/
	def_irq_handler	DebugMon_Handler	// Default_Handler
	def_irq_handler	PendSV_Handler		// Default_Handler
	def_irq_handler	SysTick_Handler		// Default_Handler
	def_irq_handler	GPIOPortA_Handler	// Default_Handler
	def_irq_handler	GPIOPortB_Handler	// Default_Handler
	def_irq_handler	GPIOPortC_Handler	// Default_Handler
	def_irq_handler	GPIOPortD_Handler	// Default_Handler
	def_irq_handler	GPIOPortE_Handler	// Default_Handler
	def_irq_handler	UART0_Handler		// Default_Handler
	def_irq_handler	UART1_Handler		// Default_Handler
	def_irq_handler	SSI0_Handler		// Default_Handler
	def_irq_handler	I2C0_Handler		// Default_Handler
	def_irq_handler	PWM0Fault_Handler	// Default_Handler
	def_irq_handler	PWM0Generator0_Handler	// Default_Handler
	def_irq_handler	PWM0Generator1_Handler	// Default_Handler
	def_irq_handler	PWM0Generator2_Handler	// Default_Handler
	def_irq_handler	Quadrature0_Handler	// Default_Handler
	def_irq_handler	ADC0Seq0_Handler	// Default_Handler
	def_irq_handler	ADC0Seq1_Handler	// Default_Handler
	def_irq_handler	ADC0Seq2_Handler	// Default_Handler
	def_irq_handler	ADC0Seq3_Handler	// Default_Handler
	def_irq_handler	WDT_Handler		// Default_Handler
	def_irq_handler	Timer0A_Handler		// Default_Handler
	def_irq_handler	Timer0B_Handler		// Default_Handler
	def_irq_handler	Timer1A_Handler		// Default_Handler
	def_irq_handler	Timer1B_Handler		// Default_Handler
	def_irq_handler	Timer2A_Handler		// Default_Handler
	def_irq_handler	Timer2B_Handler		// Default_Handler
	def_irq_handler	Comp0_Handler		// Default_Handler
	def_irq_handler	Comp1_Handler		// Default_Handler
	def_irq_handler	Comp2_Handler		// Default_Handler
	def_irq_handler	SysCtl_Handler		// Default_Handler
	def_irq_handler	FlashCtl_Handler	// Default_Handler
	def_irq_handler	GPIOPortF_Handler	// Default_Handler
	def_irq_handler	GPIOPortG_Handler	// Default_Handler
	def_irq_handler	GPIOPortH_Handler	// Default_Handler
	def_irq_handler	UART2_Handler		// Default_Handler
	def_irq_handler	SSI1_Handler		// Default_Handler
	def_irq_handler	Timer3A_Handler		// Default_Handler
	def_irq_handler	Timer3B_Handler		// Default_Handler
	def_irq_handler	I2C1_Handler		// Default_Handler
	def_irq_handler	Quadrature1_Handler	// Default_Handler
	def_irq_handler	CAN0_Handler		// Default_Handler
	def_irq_handler	CAN1_Handler		// Default_Handler
	def_irq_handler	CAN2_Handler		// Default_Handler
	def_irq_handler	Ethernet_Handler	// Default_Handler
	def_irq_handler	Hibernate_Handler	// Default_Handler
	def_irq_handler	USB0_Handler		// Default_Handler
	def_irq_handler	PWM0Generator3_Handler	// Default_Handler
	def_irq_handler	uDMA_Handler		// Default_Handler
	def_irq_handler	uDMA_Error		// Default_Handler
	def_irq_handler	ADC1Seq0_Handler	// Default_Handler
	def_irq_handler	ADC1Seq1_Handler	// Default_Handler
	def_irq_handler	ADC1Seq2_Handler	// Default_Handler
	def_irq_handler	ADC1Seq3_Handler	// Default_Handler
	def_irq_handler	I2S0_Handler		// Default_Handler
	def_irq_handler	ExtBus_Handler		// Default_Handler
	def_irq_handler	GPIOPortJ_Handler	// Default_Handler
	def_irq_handler	GPIOPortK_Handler	// Default_Handler
	def_irq_handler	GPIOPortL_Handler	// Default_Handler
	def_irq_handler	SSI2_Handler		// Default_Handler
	def_irq_handler	SSI3_Handler		// Default_Handler
	def_irq_handler	UART3_Handler		// Default_Handler
	def_irq_handler	UART4_Handler		// Default_Handler
	def_irq_handler	UART5_Handler		// Default_Handler
	def_irq_handler	UART6_Handler		// Default_Handler
	def_irq_handler	UART7_Handler		// Default_Handler
	def_irq_handler	I2C2_Handler		// Default_Handler
	def_irq_handler	I2C3_Handler		// Default_Handler
	def_irq_handler	Timer4A_Handler		// Default_Handler
	def_irq_handler	Timer4B_Handler		// Default_Handler
	def_irq_handler	Timer5A_Handler		// Default_Handler
	def_irq_handler	Timer5B_Handler		// Default_Handler
	def_irq_handler	WideTimer0A_Handler	// Default_Handler
	def_irq_handler	WideTimer0B_Handler	// Default_Handler
	def_irq_handler	WideTimer1A_Handler	// Default_Handler
	def_irq_handler	WideTimer1B_Handler	// Default_Handler
	def_irq_handler	WideTimer2A_Handler	// Default_Handler
	def_irq_handler	WideTimer2B_Handler	// Default_Handler
	def_irq_handler	WideTimer3A_Handler	// Default_Handler
	def_irq_handler	WideTimer3B_Handler	// Default_Handler
	def_irq_handler	WideTimer4A_Handler	// Default_Handler
	def_irq_handler	WideTimer4B_Handler	// Default_Handler
	def_irq_handler	WideTimer5A_Handler	// Default_Handler
	def_irq_handler	WideTimer5B_Handler	// Default_Handler
	def_irq_handler	FPU_Handler		// Default_Handler
	def_irq_handler	PECI0_Handler		// Default_Handler
	def_irq_handler	LPC0_Handler		// Default_Handler
	def_irq_handler	I2C4_Handler		// Default_Handler
	def_irq_handler	I2C5_Handler		// Default_Handler
	def_irq_handler	GPIOPortM_Handler	// Default_Handler
	def_irq_handler	GPIOPortN_Handler	// Default_Handler
	def_irq_handler	Quadrature2_Handler	// Default_Handler
	def_irq_handler	Fan0_Handler		// Default_Handler
	def_irq_handler	GPIOPortP_Handler	// Default_Handler
	def_irq_handler	GPIOPortP1_Handler	// Default_Handler
	def_irq_handler	GPIOPortP2_Handler	// Default_Handler
	def_irq_handler	GPIOPortP3_Handler	// Default_Handler
	def_irq_handler	GPIOPortP4_Handler	// Default_Handler
	def_irq_handler	GPIOPortP5_Handler	// Default_Handler
	def_irq_handler	GPIOPortP6_Handler	// Default_Handler
	def_irq_handler	GPIOPortP7_Handler	// Default_Handler
	def_irq_handler	GPIOPortQ_Handler	// Default_Handler
	def_irq_handler	GPIOPortQ1_Handler	// Default_Handler
	def_irq_handler	GPIOPortQ2_Handler	// Default_Handler
	def_irq_handler	GPIOPortQ3_Handler	// Default_Handler
	def_irq_handler	GPIOPortQ4_Handler	// Default_Handler
	def_irq_handler	GPIOPortQ5_Handler	// Default_Handler
	def_irq_handler	GPIOPortQ6_Handler	// Default_Handler
	def_irq_handler	GPIOPortQ7_Handler	// Default_Handler
	def_irq_handler	GPIOPortR_Handler	// Default_Handler
	def_irq_handler	GPIOPortS_Handler	// Default_Handler
	def_irq_handler	PWM1Generator0_Handler	// Default_Handler
	def_irq_handler	PWM1Generator1_Handler	// Default_Handler
	def_irq_handler	PWM1Generator2_Handler	// Default_Handler
	def_irq_handler	PWM1Generator3_Handler	// Default_Handler
	def_irq_handler	PWM1Fault_Handler	// Default_Handler



