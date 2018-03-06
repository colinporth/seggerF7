	.cpu cortex-m7
	.eabi_attribute 27, 1
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 1
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"ft5336.cpp"
	.text
.Ltext0:
	.section	.text._ZL25Get_I2C_InitializedStatusv,"ax",%progbits
	.align	1
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv5-sp-d16
	.type	_ZL25Get_I2C_InitializedStatusv, %function
_ZL25Get_I2C_InitializedStatusv:
.LFB0:
	.file 1 "C:\\projects\\segger\\common\\ft5336.cpp"
	.loc 1 43 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	.loc 1 46 0
	ldr	r3, .L2
	ldrb	r0, [r3]	@ zero_extendqisi2
	bx	lr
.L3:
	.align	2
.L2:
	.word	.LANCHOR0
.LFE0:
	.size	_ZL25Get_I2C_InitializedStatusv, .-_ZL25Get_I2C_InitializedStatusv
	.section	.text._ZL5resett,"ax",%progbits
	.align	1
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv5-sp-d16
	.type	_ZL5resett, %function
_ZL5resett:
.LFB5:
	.loc 1 95 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
.LVL0:
	bx	lr
.LFE5:
	.size	_ZL5resett, .-_ZL5resett
	.section	.text._ZL9tsClearITt,"ax",%progbits
	.align	1
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv5-sp-d16
	.type	_ZL9tsClearITt, %function
_ZL9tsClearITt:
.LFB11:
	.loc 1 273 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
.LVL1:
	bx	lr
.LFE11:
	.size	_ZL9tsClearITt, .-_ZL9tsClearITt
	.section	.text._ZL10tsITStatust,"ax",%progbits
	.align	1
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv5-sp-d16
	.type	_ZL10tsITStatust, %function
_ZL10tsITStatust:
.LFB12:
	.loc 1 274 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
.LVL2:
	.loc 1 274 0
	movs	r0, #0
.LVL3:
	bx	lr
.LFE12:
	.size	_ZL10tsITStatust, .-_ZL10tsITStatust
	.section	.text._ZL7tsGetXYtPtS_,"ax",%progbits
	.align	1
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv5-sp-d16
	.type	_ZL7tsGetXYtPtS_, %function
_ZL7tsGetXYtPtS_:
.LFB9:
	.loc 1 141 0
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
.LVL4:
	push	{r4, r5, r6, r7, r8, r9, r10, lr}
.LCFI0:
	sub	sp, sp, #8
.LCFI1:
	.loc 1 143 0
	movs	r3, #0
	strb	r3, [sp, #7]
.LVL5:
	.loc 1 150 0
	ldr	r4, .L23
	ldrb	r3, [r4, #2]	@ zero_extendqisi2
	ldrb	r4, [r4, #1]	@ zero_extendqisi2
	cmp	r3, r4
	bge	.L7
	.loc 1 151 0
	cmp	r3, #9
	bhi	.L9
	tbb	[pc, r3]
.L11:
	.byte	(.L21-.L11)/2
	.byte	(.L12-.L11)/2
	.byte	(.L13-.L11)/2
	.byte	(.L14-.L11)/2
	.byte	(.L15-.L11)/2
	.byte	(.L16-.L11)/2
	.byte	(.L17-.L11)/2
	.byte	(.L18-.L11)/2
	.byte	(.L19-.L11)/2
	.byte	(.L20-.L11)/2
	.p2align 1
.L9:
	.loc 1 148 0
	mov	r8, #0
	.loc 1 147 0
	mov	r9, r8
	.loc 1 146 0
	mov	r10, r8
	.loc 1 145 0
	mov	r3, r8
	b	.L10
.L12:
.LVL6:
	.loc 1 165 0
	mov	r8, #11
	.loc 1 164 0
	mov	r9, #12
	.loc 1 163 0
	mov	r10, #9
	.loc 1 162 0
	movs	r3, #10
	.loc 1 166 0
	b	.L10
.LVL7:
.L13:
	.loc 1 173 0
	mov	r8, #17
	.loc 1 172 0
	mov	r9, #18
	.loc 1 171 0
	mov	r10, #15
	.loc 1 170 0
	movs	r3, #16
	.loc 1 174 0
	b	.L10
.LVL8:
.L14:
	.loc 1 181 0
	mov	r8, #23
	.loc 1 180 0
	mov	r9, #24
	.loc 1 179 0
	mov	r10, #21
	.loc 1 178 0
	movs	r3, #22
	.loc 1 182 0
	b	.L10
.LVL9:
.L15:
	.loc 1 189 0
	mov	r8, #29
	.loc 1 188 0
	mov	r9, #30
	.loc 1 187 0
	mov	r10, #27
	.loc 1 186 0
	movs	r3, #28
	.loc 1 190 0
	b	.L10
.LVL10:
.L16:
	.loc 1 197 0
	mov	r8, #35
	.loc 1 196 0
	mov	r9, #36
	.loc 1 195 0
	mov	r10, #33
	.loc 1 194 0
	movs	r3, #34
	.loc 1 198 0
	b	.L10
.LVL11:
.L17:
	.loc 1 205 0
	mov	r8, #41
	.loc 1 204 0
	mov	r9, #42
	.loc 1 203 0
	mov	r10, #39
	.loc 1 202 0
	movs	r3, #40
	.loc 1 206 0
	b	.L10
.LVL12:
.L18:
	.loc 1 213 0
	mov	r8, #47
	.loc 1 212 0
	mov	r9, #48
	.loc 1 211 0
	mov	r10, #45
	.loc 1 210 0
	movs	r3, #46
	.loc 1 214 0
	b	.L10
.LVL13:
.L19:
	.loc 1 221 0
	mov	r8, #53
	.loc 1 220 0
	mov	r9, #54
	.loc 1 219 0
	mov	r10, #51
	.loc 1 218 0
	movs	r3, #52
	.loc 1 222 0
	b	.L10
.LVL14:
.L20:
	.loc 1 229 0
	mov	r8, #59
	.loc 1 228 0
	mov	r9, #60
	.loc 1 227 0
	mov	r10, #57
	.loc 1 226 0
	movs	r3, #58
	.loc 1 230 0
	b	.L10
.LVL15:
.L21:
	.loc 1 157 0
	mov	r8, #5
	.loc 1 156 0
	mov	r9, #6
	.loc 1 155 0
	mov	r10, #3
	.loc 1 154 0
	movs	r3, #4
.LVL16:
.L10:
	mov	r5, r2
	mov	r7, r1
.LVL17:
	.loc 1 237 0
	uxtb	r6, r0
	mov	r1, r3
.LVL18:
	mov	r0, r6
.LVL19:
	bl	TS_IO_Read
.LVL20:
	strb	r0, [sp, #7]
	.loc 1 238 0
	ldrb	r3, [sp, #7]	@ zero_extendqisi2
	ldr	r4, .L23+4
	strh	r3, [r4]	@ movhi
	.loc 1 241 0
	mov	r1, r10
	mov	r0, r6
	bl	TS_IO_Read
.LVL21:
	strb	r0, [sp, #7]
	.loc 1 242 0
	ldrb	r3, [sp, #7]	@ zero_extendqisi2
	lsls	r3, r3, #8
	and	r3, r3, #3840
	ldrh	r2, [r4]
	orrs	r3, r3, r2
	uxth	r3, r3
	strh	r3, [r4]	@ movhi
	.loc 1 245 0
	strh	r3, [r7]	@ movhi
	.loc 1 248 0
	mov	r1, r9
	mov	r0, r6
	bl	TS_IO_Read
.LVL22:
	strb	r0, [sp, #7]
	.loc 1 249 0
	ldrb	r3, [sp, #7]	@ zero_extendqisi2
	strh	r3, [r4]	@ movhi
	.loc 1 252 0
	mov	r1, r8
	mov	r0, r6
	bl	TS_IO_Read
.LVL23:
	strb	r0, [sp, #7]
	.loc 1 253 0
	ldrb	r3, [sp, #7]	@ zero_extendqisi2
	lsls	r3, r3, #8
	and	r3, r3, #3840
	ldrh	r2, [r4]
	orrs	r3, r3, r2
	uxth	r3, r3
	strh	r3, [r4]	@ movhi
	.loc 1 256 0
	strh	r3, [r5]	@ movhi
	.loc 1 258 0
	ldr	r2, .L23
	ldrb	r3, [r2, #2]	@ zero_extendqisi2
	adds	r3, r3, #1
	strb	r3, [r2, #2]
.LVL24:
.L7:
	.loc 1 260 0
	add	sp, sp, #8
.LCFI2:
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
.L24:
	.align	2
.L23:
	.word	.LANCHOR0
	.word	.LANCHOR1
.LFE9:
	.size	_ZL7tsGetXYtPtS_, .-_ZL7tsGetXYtPtS_
	.section	.text._ZL13tsDetectToucht,"ax",%progbits
	.align	1
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv5-sp-d16
	.type	_ZL13tsDetectToucht, %function
_ZL13tsDetectToucht:
.LFB8:
	.loc 1 119 0
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
.LVL25:
	push	{lr}
.LCFI3:
	sub	sp, sp, #12
.LCFI4:
	.loc 1 121 0
	movs	r3, #0
	strb	r3, [sp, #7]
	.loc 1 124 0
	movs	r1, #2
	uxtb	r0, r0
.LVL26:
	bl	TS_IO_Read
.LVL27:
	strb	r0, [sp, #7]
	.loc 1 125 0
	ldrb	r3, [sp, #7]	@ zero_extendqisi2
	and	r3, r3, #15
	strb	r3, [sp, #7]
	.loc 1 127 0
	ldrb	r3, [sp, #7]	@ zero_extendqisi2
	uxtb	r3, r3
	cmp	r3, #5
	ble	.L26
	.loc 1 129 0
	movs	r3, #0
	strb	r3, [sp, #7]
.L26:
	.loc 1 132 0
	ldrb	r2, [sp, #7]	@ zero_extendqisi2
	ldr	r3, .L28
	strb	r2, [r3, #1]
	.loc 1 135 0
	movs	r2, #0
	strb	r2, [r3, #2]
	.loc 1 137 0
	ldrb	r0, [sp, #7]	@ zero_extendqisi2
	.loc 1 138 0
	add	sp, sp, #12
.LCFI5:
	@ sp needed
	ldr	pc, [sp], #4
.L29:
	.align	2
.L28:
	.word	.LANCHOR0
.LFE8:
	.size	_ZL13tsDetectToucht, .-_ZL13tsDetectToucht
	.section	.text._ZL11tsDisableITt,"ax",%progbits
	.align	1
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv5-sp-d16
	.type	_ZL11tsDisableITt, %function
_ZL11tsDisableITt:
.LFB6:
	.loc 1 98 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
.LVL28:
	push	{r3, lr}
.LCFI6:
.LVL29:
	.loc 1 104 0
	movs	r2, #0
	movs	r1, #164
	uxtb	r0, r0
.LVL30:
	bl	TS_IO_Write
.LVL31:
	pop	{r3, pc}
.LFE6:
	.size	_ZL11tsDisableITt, .-_ZL11tsDisableITt
	.section	.text._ZL7tsStartt,"ax",%progbits
	.align	1
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv5-sp-d16
	.type	_ZL7tsStartt, %function
_ZL7tsStartt:
.LFB7:
	.loc 1 108 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
.LVL32:
	push	{r3, lr}
.LCFI7:
	.loc 1 115 0
	bl	_ZL11tsDisableITt
.LVL33:
	pop	{r3, pc}
.LFE7:
	.size	_ZL7tsStartt, .-_ZL7tsStartt
	.section	.text._ZL10tsEnableITt,"ax",%progbits
	.align	1
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv5-sp-d16
	.type	_ZL10tsEnableITt, %function
_ZL10tsEnableITt:
.LFB10:
	.loc 1 264 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
.LVL34:
	push	{r3, lr}
.LCFI8:
.LVL35:
	.loc 1 270 0
	movs	r2, #1
	movs	r1, #164
	uxtb	r0, r0
.LVL36:
	bl	TS_IO_Write
.LVL37:
	pop	{r3, pc}
.LFE10:
	.size	_ZL10tsEnableITt, .-_ZL10tsEnableITt
	.section	.text._ZL24I2C_InitializeIfRequiredv,"ax",%progbits
	.align	1
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv5-sp-d16
	.type	_ZL24I2C_InitializeIfRequiredv, %function
_ZL24I2C_InitializeIfRequiredv:
.LFB1:
	.loc 1 49 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
.LCFI9:
	.loc 1 51 0
	bl	_ZL25Get_I2C_InitializedStatusv
.LVL38:
	cbz	r0, .L39
.L36:
	pop	{r3, pc}
.L39:
	.loc 1 52 0
	bl	TS_IO_Init
.LVL39:
	.loc 1 53 0
	movs	r2, #1
	ldr	r3, .L40
	strb	r2, [r3]
	.loc 1 55 0
	b	.L36
.L41:
	.align	2
.L40:
	.word	.LANCHOR0
.LFE1:
	.size	_ZL24I2C_InitializeIfRequiredv, .-_ZL24I2C_InitializeIfRequiredv
	.section	.text._ZL6readIdt,"ax",%progbits
	.align	1
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv5-sp-d16
	.type	_ZL6readIdt, %function
_ZL6readIdt:
.LFB4:
	.loc 1 71 0
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
.LVL40:
	push	{r4, r5, r6, lr}
.LCFI10:
	sub	sp, sp, #8
.LCFI11:
	mov	r6, r0
	.loc 1 73 0
	movs	r4, #0
	strb	r4, [sp, #7]
.LVL41:
	.loc 1 78 0
	bl	_ZL24I2C_InitializeIfRequiredv
.LVL42:
	.loc 1 75 0
	mov	r5, r4
	b	.L45
.LVL43:
.L44:
	.loc 1 81 0 discriminator 3
	adds	r4, r4, #1
.LVL44:
	uxtb	r4, r4
.LVL45:
.L45:
	.loc 1 81 0 is_stmt 0 discriminator 1
	cmp	r4, #2
	bgt	.L43
	.loc 1 81 0 discriminator 2
	cbnz	r5, .L43
	.loc 1 83 0 is_stmt 1
	movs	r1, #168
	uxtb	r0, r6
	bl	TS_IO_Read
.LVL46:
	strb	r0, [sp, #7]
	.loc 1 86 0
	ldrb	r3, [sp, #7]	@ zero_extendqisi2
	uxtb	r3, r3
	cmp	r3, #81
	bne	.L44
	.loc 1 88 0
	movs	r5, #1
.LVL47:
	b	.L44
.LVL48:
.L43:
	.loc 1 92 0
	ldrb	r0, [sp, #7]	@ zero_extendqisi2
	.loc 1 93 0
	add	sp, sp, #8
.LCFI12:
	@ sp needed
	pop	{r4, r5, r6, pc}
.LFE4:
	.size	_ZL6readIdt, .-_ZL6readIdt
	.section	.text._ZL4initt,"ax",%progbits
	.align	1
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv5-sp-d16
	.type	_ZL4initt, %function
_ZL4initt:
.LFB3:
	.loc 1 60 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
.LVL49:
	push	{r3, lr}
.LCFI13:
	.loc 1 64 0
	movs	r0, #200
.LVL50:
	bl	TS_IO_Delay
.LVL51:
	.loc 1 67 0
	bl	_ZL24I2C_InitializeIfRequiredv
.LVL52:
	pop	{r3, pc}
.LFE3:
	.size	_ZL4initt, .-_ZL4initt
	.section	.text.ft5336_TS_GetTouchInfo,"ax",%progbits
	.align	1
	.global	ft5336_TS_GetTouchInfo
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv5-sp-d16
	.type	ft5336_TS_GetTouchInfo, %function
ft5336_TS_GetTouchInfo:
.LFB13:
	.loc 1 278 0
	@ args = 4, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
.LVL53:
	push	{r4, r5, r6, r7, r8, lr}
.LCFI14:
	sub	sp, sp, #8
.LCFI15:
	.loc 1 280 0
	movs	r4, #0
	strb	r4, [sp, #7]
.LVL54:
	.loc 1 285 0
	ldr	r4, .L66
	ldrb	r4, [r4, #1]	@ zero_extendqisi2
	cmp	r4, r1
	bls	.L50
	.loc 1 286 0
	cmp	r1, #9
	bhi	.L52
	tbb	[pc, r1]
.L54:
	.byte	(.L64-.L54)/2
	.byte	(.L55-.L54)/2
	.byte	(.L56-.L54)/2
	.byte	(.L57-.L54)/2
	.byte	(.L58-.L54)/2
	.byte	(.L59-.L54)/2
	.byte	(.L60-.L54)/2
	.byte	(.L61-.L54)/2
	.byte	(.L62-.L54)/2
	.byte	(.L63-.L54)/2
	.p2align 1
.L52:
	.loc 1 283 0
	movs	r7, #0
	.loc 1 282 0
	mov	r8, r7
	.loc 1 281 0
	mov	r1, r7
.LVL55:
	b	.L53
.LVL56:
.L55:
	.loc 1 298 0
	movs	r7, #14
	.loc 1 297 0
	mov	r8, #13
	.loc 1 296 0
	movs	r1, #9
.LVL57:
	.loc 1 299 0
	b	.L53
.LVL58:
.L56:
	.loc 1 305 0
	movs	r7, #20
	.loc 1 304 0
	mov	r8, #19
	.loc 1 303 0
	movs	r1, #15
.LVL59:
	.loc 1 306 0
	b	.L53
.LVL60:
.L57:
	.loc 1 312 0
	movs	r7, #26
	.loc 1 311 0
	mov	r8, #25
	.loc 1 310 0
	movs	r1, #21
.LVL61:
	.loc 1 313 0
	b	.L53
.LVL62:
.L58:
	.loc 1 319 0
	movs	r7, #32
	.loc 1 318 0
	mov	r8, #31
	.loc 1 317 0
	movs	r1, #27
.LVL63:
	.loc 1 320 0
	b	.L53
.LVL64:
.L59:
	.loc 1 326 0
	movs	r7, #38
	.loc 1 325 0
	mov	r8, #37
	.loc 1 324 0
	movs	r1, #33
.LVL65:
	.loc 1 327 0
	b	.L53
.LVL66:
.L60:
	.loc 1 333 0
	movs	r7, #44
	.loc 1 332 0
	mov	r8, #43
	.loc 1 331 0
	movs	r1, #39
.LVL67:
	.loc 1 334 0
	b	.L53
.LVL68:
.L61:
	.loc 1 340 0
	movs	r7, #50
	.loc 1 339 0
	mov	r8, #49
	.loc 1 338 0
	movs	r1, #45
.LVL69:
	.loc 1 341 0
	b	.L53
.LVL70:
.L62:
	.loc 1 347 0
	movs	r7, #56
	.loc 1 346 0
	mov	r8, #55
	.loc 1 345 0
	movs	r1, #51
.LVL71:
	.loc 1 348 0
	b	.L53
.LVL72:
.L63:
	.loc 1 354 0
	movs	r7, #62
	.loc 1 353 0
	mov	r8, #61
	.loc 1 352 0
	movs	r1, #57
.LVL73:
	.loc 1 355 0
	b	.L53
.LVL74:
.L64:
	.loc 1 291 0
	movs	r7, #8
	.loc 1 290 0
	mov	r8, #7
	.loc 1 289 0
	movs	r1, #3
.LVL75:
.L53:
	mov	r5, r3
	mov	r6, r2
.LVL76:
	.loc 1 362 0
	uxtb	r4, r0
	mov	r0, r4
.LVL77:
	bl	TS_IO_Read
.LVL78:
	strb	r0, [sp, #7]
	.loc 1 363 0
	ldrb	r3, [sp, #7]	@ zero_extendqisi2
	lsrs	r3, r3, #6
	ldr	r2, [sp, #32]
	str	r3, [r2]
	.loc 1 366 0
	mov	r1, r8
	mov	r0, r4
	bl	TS_IO_Read
.LVL79:
	strb	r0, [sp, #7]
	.loc 1 367 0
	ldrb	r3, [sp, #7]	@ zero_extendqisi2
	str	r3, [r6]
	.loc 1 370 0
	mov	r1, r7
	mov	r0, r4
	bl	TS_IO_Read
.LVL80:
	strb	r0, [sp, #7]
	.loc 1 371 0
	ldrb	r2, [sp, #7]	@ zero_extendqisi2
	lsrs	r2, r2, #4
	and	r2, r2, #4
	str	r2, [r5]
.LVL81:
.L50:
	.loc 1 373 0
	add	sp, sp, #8
.LCFI16:
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.LVL82:
.L67:
	.align	2
.L66:
	.word	.LANCHOR0
.LFE13:
	.size	ft5336_TS_GetTouchInfo, .-ft5336_TS_GetTouchInfo
	.section	.text.ft5336_TS_GetGestureID,"ax",%progbits
	.align	1
	.global	ft5336_TS_GetGestureID
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv5-sp-d16
	.type	ft5336_TS_GetGestureID, %function
ft5336_TS_GetGestureID:
.LFB14:
	.loc 1 376 0
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
.LVL83:
	push	{r4, lr}
.LCFI17:
	sub	sp, sp, #8
.LCFI18:
	mov	r4, r1
	.loc 1 378 0
	movs	r3, #0
	strb	r3, [sp, #7]
	.loc 1 379 0
	movs	r1, #1
.LVL84:
	uxtb	r0, r0
.LVL85:
	bl	TS_IO_Read
.LVL86:
	strb	r0, [sp, #7]
	.loc 1 380 0
	ldrb	r3, [sp, #7]	@ zero_extendqisi2
	str	r3, [r4]
	.loc 1 381 0
	add	sp, sp, #8
.LCFI19:
	@ sp needed
	pop	{r4, pc}
.LFE14:
	.size	ft5336_TS_GetGestureID, .-ft5336_TS_GetGestureID
	.global	ft5336_ts_drv
	.section	.bss._ZL9gTsHandle,"aw",%nobits
	.align	2
	.set	.LANCHOR0,. + 0
	.type	_ZL9gTsHandle, %object
	.size	_ZL9gTsHandle, 3
_ZL9gTsHandle:
	.space	3
	.section	.bss._ZZL7tsGetXYtPtS_E5coord,"aw",%nobits
	.align	1
	.set	.LANCHOR1,. + 0
	.type	_ZZL7tsGetXYtPtS_E5coord, %object
	.size	_ZZL7tsGetXYtPtS_E5coord, 2
_ZZL7tsGetXYtPtS_E5coord:
	.space	2
	.section	.data.ft5336_ts_drv,"aw",%progbits
	.align	2
	.type	ft5336_ts_drv, %object
	.size	ft5336_ts_drv, 40
ft5336_ts_drv:
	.word	_ZL4initt
	.word	_ZL6readIdt
	.word	_ZL5resett
	.word	_ZL7tsStartt
	.word	_ZL13tsDetectToucht
	.word	_ZL7tsGetXYtPtS_
	.word	_ZL10tsEnableITt
	.word	_ZL9tsClearITt
	.word	_ZL10tsITStatust
	.word	_ZL11tsDisableITt
	.section	.debug_frame,"",%progbits
.Lframe0:
	.4byte	.LECIE0-.LSCIE0
.LSCIE0:
	.4byte	0xffffffff
	.byte	0x3
	.ascii	"\000"
	.uleb128 0x1
	.sleb128 -4
	.uleb128 0xe
	.byte	0xc
	.uleb128 0xd
	.uleb128 0
	.align	2
.LECIE0:
.LSFDE0:
	.4byte	.LEFDE0-.LASFDE0
.LASFDE0:
	.4byte	.Lframe0
	.4byte	.LFB0
	.4byte	.LFE0-.LFB0
	.align	2
.LEFDE0:
.LSFDE2:
	.4byte	.LEFDE2-.LASFDE2
.LASFDE2:
	.4byte	.Lframe0
	.4byte	.LFB5
	.4byte	.LFE5-.LFB5
	.align	2
.LEFDE2:
.LSFDE4:
	.4byte	.LEFDE4-.LASFDE4
.LASFDE4:
	.4byte	.Lframe0
	.4byte	.LFB11
	.4byte	.LFE11-.LFB11
	.align	2
.LEFDE4:
.LSFDE6:
	.4byte	.LEFDE6-.LASFDE6
.LASFDE6:
	.4byte	.Lframe0
	.4byte	.LFB12
	.4byte	.LFE12-.LFB12
	.align	2
.LEFDE6:
.LSFDE8:
	.4byte	.LEFDE8-.LASFDE8
.LASFDE8:
	.4byte	.Lframe0
	.4byte	.LFB9
	.4byte	.LFE9-.LFB9
	.byte	0x4
	.4byte	.LCFI0-.LFB9
	.byte	0xe
	.uleb128 0x20
	.byte	0x84
	.uleb128 0x8
	.byte	0x85
	.uleb128 0x7
	.byte	0x86
	.uleb128 0x6
	.byte	0x87
	.uleb128 0x5
	.byte	0x88
	.uleb128 0x4
	.byte	0x89
	.uleb128 0x3
	.byte	0x8a
	.uleb128 0x2
	.byte	0x8e
	.uleb128 0x1
	.byte	0x4
	.4byte	.LCFI1-.LCFI0
	.byte	0xe
	.uleb128 0x28
	.byte	0x4
	.4byte	.LCFI2-.LCFI1
	.byte	0xe
	.uleb128 0x20
	.align	2
.LEFDE8:
.LSFDE10:
	.4byte	.LEFDE10-.LASFDE10
.LASFDE10:
	.4byte	.Lframe0
	.4byte	.LFB8
	.4byte	.LFE8-.LFB8
	.byte	0x4
	.4byte	.LCFI3-.LFB8
	.byte	0xe
	.uleb128 0x4
	.byte	0x8e
	.uleb128 0x1
	.byte	0x4
	.4byte	.LCFI4-.LCFI3
	.byte	0xe
	.uleb128 0x10
	.byte	0x4
	.4byte	.LCFI5-.LCFI4
	.byte	0xe
	.uleb128 0x4
	.align	2
.LEFDE10:
.LSFDE12:
	.4byte	.LEFDE12-.LASFDE12
.LASFDE12:
	.4byte	.Lframe0
	.4byte	.LFB6
	.4byte	.LFE6-.LFB6
	.byte	0x4
	.4byte	.LCFI6-.LFB6
	.byte	0xe
	.uleb128 0x8
	.byte	0x83
	.uleb128 0x2
	.byte	0x8e
	.uleb128 0x1
	.align	2
.LEFDE12:
.LSFDE14:
	.4byte	.LEFDE14-.LASFDE14
.LASFDE14:
	.4byte	.Lframe0
	.4byte	.LFB7
	.4byte	.LFE7-.LFB7
	.byte	0x4
	.4byte	.LCFI7-.LFB7
	.byte	0xe
	.uleb128 0x8
	.byte	0x83
	.uleb128 0x2
	.byte	0x8e
	.uleb128 0x1
	.align	2
.LEFDE14:
.LSFDE16:
	.4byte	.LEFDE16-.LASFDE16
.LASFDE16:
	.4byte	.Lframe0
	.4byte	.LFB10
	.4byte	.LFE10-.LFB10
	.byte	0x4
	.4byte	.LCFI8-.LFB10
	.byte	0xe
	.uleb128 0x8
	.byte	0x83
	.uleb128 0x2
	.byte	0x8e
	.uleb128 0x1
	.align	2
.LEFDE16:
.LSFDE18:
	.4byte	.LEFDE18-.LASFDE18
.LASFDE18:
	.4byte	.Lframe0
	.4byte	.LFB1
	.4byte	.LFE1-.LFB1
	.byte	0x4
	.4byte	.LCFI9-.LFB1
	.byte	0xe
	.uleb128 0x8
	.byte	0x83
	.uleb128 0x2
	.byte	0x8e
	.uleb128 0x1
	.align	2
.LEFDE18:
.LSFDE20:
	.4byte	.LEFDE20-.LASFDE20
.LASFDE20:
	.4byte	.Lframe0
	.4byte	.LFB4
	.4byte	.LFE4-.LFB4
	.byte	0x4
	.4byte	.LCFI10-.LFB4
	.byte	0xe
	.uleb128 0x10
	.byte	0x84
	.uleb128 0x4
	.byte	0x85
	.uleb128 0x3
	.byte	0x86
	.uleb128 0x2
	.byte	0x8e
	.uleb128 0x1
	.byte	0x4
	.4byte	.LCFI11-.LCFI10
	.byte	0xe
	.uleb128 0x18
	.byte	0x4
	.4byte	.LCFI12-.LCFI11
	.byte	0xe
	.uleb128 0x10
	.align	2
.LEFDE20:
.LSFDE22:
	.4byte	.LEFDE22-.LASFDE22
.LASFDE22:
	.4byte	.Lframe0
	.4byte	.LFB3
	.4byte	.LFE3-.LFB3
	.byte	0x4
	.4byte	.LCFI13-.LFB3
	.byte	0xe
	.uleb128 0x8
	.byte	0x83
	.uleb128 0x2
	.byte	0x8e
	.uleb128 0x1
	.align	2
.LEFDE22:
.LSFDE24:
	.4byte	.LEFDE24-.LASFDE24
.LASFDE24:
	.4byte	.Lframe0
	.4byte	.LFB13
	.4byte	.LFE13-.LFB13
	.byte	0x4
	.4byte	.LCFI14-.LFB13
	.byte	0xe
	.uleb128 0x18
	.byte	0x84
	.uleb128 0x6
	.byte	0x85
	.uleb128 0x5
	.byte	0x86
	.uleb128 0x4
	.byte	0x87
	.uleb128 0x3
	.byte	0x88
	.uleb128 0x2
	.byte	0x8e
	.uleb128 0x1
	.byte	0x4
	.4byte	.LCFI15-.LCFI14
	.byte	0xe
	.uleb128 0x20
	.byte	0x4
	.4byte	.LCFI16-.LCFI15
	.byte	0xe
	.uleb128 0x18
	.align	2
.LEFDE24:
.LSFDE26:
	.4byte	.LEFDE26-.LASFDE26
.LASFDE26:
	.4byte	.Lframe0
	.4byte	.LFB14
	.4byte	.LFE14-.LFB14
	.byte	0x4
	.4byte	.LCFI17-.LFB14
	.byte	0xe
	.uleb128 0x8
	.byte	0x84
	.uleb128 0x2
	.byte	0x8e
	.uleb128 0x1
	.byte	0x4
	.4byte	.LCFI18-.LCFI17
	.byte	0xe
	.uleb128 0x10
	.byte	0x4
	.4byte	.LCFI19-.LCFI18
	.byte	0xe
	.uleb128 0x8
	.align	2
.LEFDE26:
	.text
.Letext0:
	.file 2 "C:/Program Files/SEGGER/SEGGER Embedded Studio for ARM 3.34a/include/stdint.h"
	.file 3 "C:\\projects\\segger\\common\\ts.h"
	.file 4 "C:\\projects\\segger\\common\\ft5336.h"
	.section	.debug_info,"",%progbits
.Ldebug_info0:
	.4byte	0x6e0
	.2byte	0x4
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.LASF660
	.byte	0x4
	.4byte	.LASF661
	.4byte	.Ldebug_ranges0+0
	.4byte	0
	.4byte	.Ldebug_line0
	.4byte	.Ldebug_macro0
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.4byte	.LASF596
	.uleb128 0x3
	.4byte	.LASF599
	.byte	0x2
	.byte	0x10
	.4byte	0x3c
	.uleb128 0x4
	.4byte	0x2c
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.4byte	.LASF597
	.uleb128 0x2
	.byte	0x2
	.byte	0x5
	.4byte	.LASF598
	.uleb128 0x3
	.4byte	.LASF600
	.byte	0x2
	.byte	0x16
	.4byte	0x55
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.4byte	.LASF601
	.uleb128 0x5
	.byte	0x4
	.byte	0x5
	.ascii	"int\000"
	.uleb128 0x3
	.4byte	.LASF602
	.byte	0x2
	.byte	0x18
	.4byte	0x6e
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF603
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.4byte	.LASF604
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.4byte	.LASF605
	.uleb128 0x6
	.byte	0x28
	.byte	0x3
	.byte	0x45
	.4byte	.LASF617
	.4byte	0x108
	.uleb128 0x7
	.4byte	.LASF606
	.byte	0x3
	.byte	0x46
	.4byte	0x113
	.byte	0
	.uleb128 0x7
	.4byte	.LASF607
	.byte	0x3
	.byte	0x47
	.4byte	0x128
	.byte	0x4
	.uleb128 0x7
	.4byte	.LASF608
	.byte	0x3
	.byte	0x48
	.4byte	0x113
	.byte	0x8
	.uleb128 0x7
	.4byte	.LASF609
	.byte	0x3
	.byte	0x49
	.4byte	0x113
	.byte	0xc
	.uleb128 0x7
	.4byte	.LASF610
	.byte	0x3
	.byte	0x4a
	.4byte	0x13d
	.byte	0x10
	.uleb128 0x7
	.4byte	.LASF611
	.byte	0x3
	.byte	0x4b
	.4byte	0x15e
	.byte	0x14
	.uleb128 0x7
	.4byte	.LASF612
	.byte	0x3
	.byte	0x4c
	.4byte	0x113
	.byte	0x18
	.uleb128 0x7
	.4byte	.LASF613
	.byte	0x3
	.byte	0x4d
	.4byte	0x113
	.byte	0x1c
	.uleb128 0x7
	.4byte	.LASF614
	.byte	0x3
	.byte	0x4e
	.4byte	0x13d
	.byte	0x20
	.uleb128 0x7
	.4byte	.LASF615
	.byte	0x3
	.byte	0x4f
	.4byte	0x113
	.byte	0x24
	.byte	0
	.uleb128 0x8
	.4byte	0x113
	.uleb128 0x9
	.4byte	0x4a
	.byte	0
	.uleb128 0xa
	.byte	0x4
	.4byte	0x108
	.uleb128 0xb
	.4byte	0x4a
	.4byte	0x128
	.uleb128 0x9
	.4byte	0x4a
	.byte	0
	.uleb128 0xa
	.byte	0x4
	.4byte	0x119
	.uleb128 0xb
	.4byte	0x2c
	.4byte	0x13d
	.uleb128 0x9
	.4byte	0x4a
	.byte	0
	.uleb128 0xa
	.byte	0x4
	.4byte	0x12e
	.uleb128 0x8
	.4byte	0x158
	.uleb128 0x9
	.4byte	0x4a
	.uleb128 0x9
	.4byte	0x158
	.uleb128 0x9
	.4byte	0x158
	.byte	0
	.uleb128 0xa
	.byte	0x4
	.4byte	0x4a
	.uleb128 0xa
	.byte	0x4
	.4byte	0x143
	.uleb128 0x3
	.4byte	.LASF616
	.byte	0x3
	.byte	0x50
	.4byte	0x83
	.uleb128 0x6
	.byte	0x3
	.byte	0x4
	.byte	0x36
	.4byte	.LASF618
	.4byte	0x1a0
	.uleb128 0x7
	.4byte	.LASF619
	.byte	0x4
	.byte	0x37
	.4byte	0x2c
	.byte	0
	.uleb128 0x7
	.4byte	.LASF620
	.byte	0x4
	.byte	0x3a
	.4byte	0x2c
	.byte	0x1
	.uleb128 0x7
	.4byte	.LASF621
	.byte	0x4
	.byte	0x3d
	.4byte	0x2c
	.byte	0x2
	.byte	0
	.uleb128 0x3
	.4byte	.LASF622
	.byte	0x4
	.byte	0x3f
	.4byte	0x16f
	.uleb128 0xc
	.4byte	.LASF662
	.byte	0x4
	.2byte	0x135
	.4byte	0x164
	.uleb128 0xd
	.4byte	.LASF625
	.byte	0x1
	.byte	0x28
	.4byte	0x1a0
	.uleb128 0x5
	.byte	0x3
	.4byte	_ZL9gTsHandle
	.uleb128 0xe
	.4byte	0x1ab
	.byte	0x1
	.2byte	0x180
	.uleb128 0x5
	.byte	0x3
	.4byte	ft5336_ts_drv
	.uleb128 0xf
	.4byte	.LASF627
	.byte	0x1
	.2byte	0x178
	.4byte	.LFB14
	.4byte	.LFE14-.LFB14
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x232
	.uleb128 0x10
	.4byte	.LASF623
	.byte	0x1
	.2byte	0x178
	.4byte	0x4a
	.4byte	.LLST24
	.uleb128 0x10
	.4byte	.LASF624
	.byte	0x1
	.2byte	0x178
	.4byte	0x232
	.4byte	.LLST25
	.uleb128 0x11
	.4byte	.LASF626
	.byte	0x1
	.2byte	0x17a
	.4byte	0x37
	.uleb128 0x2
	.byte	0x91
	.sleb128 -9
	.uleb128 0x12
	.4byte	.LVL86
	.4byte	0x6b3
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x50
	.uleb128 0x3
	.byte	0xf3
	.uleb128 0x1
	.byte	0x50
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x1
	.byte	0x31
	.byte	0
	.byte	0
	.uleb128 0xa
	.byte	0x4
	.4byte	0x63
	.uleb128 0xf
	.4byte	.LASF628
	.byte	0x1
	.2byte	0x115
	.4byte	.LFB13
	.4byte	.LFE13-.LFB13
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x322
	.uleb128 0x10
	.4byte	.LASF623
	.byte	0x1
	.2byte	0x115
	.4byte	0x4a
	.4byte	.LLST16
	.uleb128 0x10
	.4byte	.LASF629
	.byte	0x1
	.2byte	0x115
	.4byte	0x63
	.4byte	.LLST17
	.uleb128 0x10
	.4byte	.LASF630
	.byte	0x1
	.2byte	0x116
	.4byte	0x232
	.4byte	.LLST18
	.uleb128 0x10
	.4byte	.LASF631
	.byte	0x1
	.2byte	0x116
	.4byte	0x232
	.4byte	.LLST19
	.uleb128 0x10
	.4byte	.LASF632
	.byte	0x1
	.2byte	0x116
	.4byte	0x232
	.4byte	.LLST20
	.uleb128 0x11
	.4byte	.LASF626
	.byte	0x1
	.2byte	0x118
	.4byte	0x37
	.uleb128 0x2
	.byte	0x91
	.sleb128 -25
	.uleb128 0x14
	.4byte	.LASF633
	.byte	0x1
	.2byte	0x119
	.4byte	0x2c
	.4byte	.LLST21
	.uleb128 0x14
	.4byte	.LASF634
	.byte	0x1
	.2byte	0x11a
	.4byte	0x2c
	.4byte	.LLST22
	.uleb128 0x14
	.4byte	.LASF635
	.byte	0x1
	.2byte	0x11b
	.4byte	0x2c
	.4byte	.LLST23
	.uleb128 0x15
	.4byte	.LVL78
	.4byte	0x6b3
	.4byte	0x2f1
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x50
	.uleb128 0x2
	.byte	0x74
	.sleb128 0
	.byte	0
	.uleb128 0x15
	.4byte	.LVL79
	.4byte	0x6b3
	.4byte	0x30b
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x50
	.uleb128 0x2
	.byte	0x74
	.sleb128 0
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x2
	.byte	0x78
	.sleb128 0
	.byte	0
	.uleb128 0x12
	.4byte	.LVL80
	.4byte	0x6b3
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x50
	.uleb128 0x2
	.byte	0x74
	.sleb128 0
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x2
	.byte	0x77
	.sleb128 0
	.byte	0
	.byte	0
	.uleb128 0x16
	.4byte	.LASF644
	.byte	0x1
	.2byte	0x112
	.4byte	0x2c
	.4byte	.LFB12
	.4byte	.LFE12-.LFB12
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x34d
	.uleb128 0x10
	.4byte	.LASF623
	.byte	0x1
	.2byte	0x112
	.4byte	0x4a
	.4byte	.LLST0
	.byte	0
	.uleb128 0x17
	.4byte	.LASF636
	.byte	0x1
	.2byte	0x111
	.4byte	.LFB11
	.4byte	.LFE11-.LFB11
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x372
	.uleb128 0x18
	.4byte	.LASF623
	.byte	0x1
	.2byte	0x111
	.4byte	0x4a
	.uleb128 0x1
	.byte	0x50
	.byte	0
	.uleb128 0x17
	.4byte	.LASF637
	.byte	0x1
	.2byte	0x108
	.4byte	.LFB10
	.4byte	.LFE10-.LFB10
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x3c2
	.uleb128 0x10
	.4byte	.LASF623
	.byte	0x1
	.2byte	0x108
	.4byte	0x4a
	.4byte	.LLST11
	.uleb128 0x19
	.4byte	.LASF638
	.byte	0x1
	.2byte	0x10a
	.4byte	0x2c
	.byte	0x1
	.uleb128 0x12
	.4byte	.LVL37
	.4byte	0x6bf
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x50
	.uleb128 0x3
	.byte	0xf3
	.uleb128 0x1
	.byte	0x50
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x2
	.byte	0x9
	.byte	0xa4
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x52
	.uleb128 0x1
	.byte	0x31
	.byte	0
	.byte	0
	.uleb128 0x1a
	.4byte	.LASF639
	.byte	0x1
	.byte	0x8d
	.4byte	.LFB9
	.4byte	.LFE9-.LFB9
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x4ba
	.uleb128 0x1b
	.4byte	.LASF623
	.byte	0x1
	.byte	0x8d
	.4byte	0x4a
	.4byte	.LLST1
	.uleb128 0x1c
	.ascii	"X\000"
	.byte	0x1
	.byte	0x8d
	.4byte	0x158
	.4byte	.LLST2
	.uleb128 0x1c
	.ascii	"Y\000"
	.byte	0x1
	.byte	0x8d
	.4byte	0x158
	.4byte	.LLST3
	.uleb128 0xd
	.4byte	.LASF626
	.byte	0x1
	.byte	0x8f
	.4byte	0x37
	.uleb128 0x2
	.byte	0x91
	.sleb128 -33
	.uleb128 0xd
	.4byte	.LASF640
	.byte	0x1
	.byte	0x90
	.4byte	0x4a
	.uleb128 0x5
	.byte	0x3
	.4byte	_ZZL7tsGetXYtPtS_E5coord
	.uleb128 0x1d
	.4byte	.LASF641
	.byte	0x1
	.byte	0x91
	.4byte	0x2c
	.4byte	.LLST4
	.uleb128 0x1d
	.4byte	.LASF633
	.byte	0x1
	.byte	0x92
	.4byte	0x2c
	.4byte	.LLST5
	.uleb128 0x1d
	.4byte	.LASF642
	.byte	0x1
	.byte	0x93
	.4byte	0x2c
	.4byte	.LLST6
	.uleb128 0x1d
	.4byte	.LASF643
	.byte	0x1
	.byte	0x94
	.4byte	0x2c
	.4byte	.LLST7
	.uleb128 0x15
	.4byte	.LVL20
	.4byte	0x6b3
	.4byte	0x46f
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x50
	.uleb128 0x2
	.byte	0x76
	.sleb128 0
	.byte	0
	.uleb128 0x15
	.4byte	.LVL21
	.4byte	0x6b3
	.4byte	0x489
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x50
	.uleb128 0x2
	.byte	0x76
	.sleb128 0
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x2
	.byte	0x7a
	.sleb128 0
	.byte	0
	.uleb128 0x15
	.4byte	.LVL22
	.4byte	0x6b3
	.4byte	0x4a3
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x50
	.uleb128 0x2
	.byte	0x76
	.sleb128 0
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x2
	.byte	0x79
	.sleb128 0
	.byte	0
	.uleb128 0x12
	.4byte	.LVL23
	.4byte	0x6b3
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x50
	.uleb128 0x2
	.byte	0x76
	.sleb128 0
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x2
	.byte	0x78
	.sleb128 0
	.byte	0
	.byte	0
	.uleb128 0x1e
	.4byte	.LASF645
	.byte	0x1
	.byte	0x77
	.4byte	0x2c
	.4byte	.LFB8
	.4byte	.LFE8-.LFB8
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x507
	.uleb128 0x1b
	.4byte	.LASF623
	.byte	0x1
	.byte	0x77
	.4byte	0x4a
	.4byte	.LLST8
	.uleb128 0xd
	.4byte	.LASF646
	.byte	0x1
	.byte	0x79
	.4byte	0x37
	.uleb128 0x2
	.byte	0x91
	.sleb128 -9
	.uleb128 0x12
	.4byte	.LVL27
	.4byte	0x6b3
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x50
	.uleb128 0x3
	.byte	0xf3
	.uleb128 0x1
	.byte	0x50
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x1
	.byte	0x32
	.byte	0
	.byte	0
	.uleb128 0x1a
	.4byte	.LASF647
	.byte	0x1
	.byte	0x6c
	.4byte	.LFB7
	.4byte	.LFE7-.LFB7
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x535
	.uleb128 0x1b
	.4byte	.LASF623
	.byte	0x1
	.byte	0x6c
	.4byte	0x4a
	.4byte	.LLST10
	.uleb128 0x1f
	.4byte	.LVL33
	.4byte	0x535
	.byte	0
	.uleb128 0x1a
	.4byte	.LASF648
	.byte	0x1
	.byte	0x62
	.4byte	.LFB6
	.4byte	.LFE6-.LFB6
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x582
	.uleb128 0x1b
	.4byte	.LASF623
	.byte	0x1
	.byte	0x62
	.4byte	0x4a
	.4byte	.LLST9
	.uleb128 0x20
	.4byte	.LASF638
	.byte	0x1
	.byte	0x64
	.4byte	0x2c
	.byte	0
	.uleb128 0x12
	.4byte	.LVL31
	.4byte	0x6bf
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x50
	.uleb128 0x3
	.byte	0xf3
	.uleb128 0x1
	.byte	0x50
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x2
	.byte	0x9
	.byte	0xa4
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x52
	.uleb128 0x1
	.byte	0x30
	.byte	0
	.byte	0
	.uleb128 0x1a
	.4byte	.LASF649
	.byte	0x1
	.byte	0x5f
	.4byte	.LFB5
	.4byte	.LFE5-.LFB5
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x5a5
	.uleb128 0x21
	.4byte	.LASF623
	.byte	0x1
	.byte	0x5f
	.4byte	0x4a
	.uleb128 0x1
	.byte	0x50
	.byte	0
	.uleb128 0x1e
	.4byte	.LASF650
	.byte	0x1
	.byte	0x47
	.4byte	0x4a
	.4byte	.LFB4
	.4byte	.LFE4-.LFB4
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x619
	.uleb128 0x1b
	.4byte	.LASF623
	.byte	0x1
	.byte	0x47
	.4byte	0x4a
	.4byte	.LLST12
	.uleb128 0xd
	.4byte	.LASF651
	.byte	0x1
	.byte	0x49
	.4byte	0x37
	.uleb128 0x2
	.byte	0x91
	.sleb128 -17
	.uleb128 0x1d
	.4byte	.LASF652
	.byte	0x1
	.byte	0x4a
	.4byte	0x2c
	.4byte	.LLST13
	.uleb128 0x1d
	.4byte	.LASF653
	.byte	0x1
	.byte	0x4b
	.4byte	0x2c
	.4byte	.LLST14
	.uleb128 0x1f
	.4byte	.LVL42
	.4byte	0x676
	.uleb128 0x12
	.4byte	.LVL46
	.4byte	0x6b3
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x50
	.uleb128 0x2
	.byte	0x76
	.sleb128 0
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x2
	.byte	0x9
	.byte	0xa8
	.byte	0
	.byte	0
	.uleb128 0x1a
	.4byte	.LASF654
	.byte	0x1
	.byte	0x3c
	.4byte	.LFB3
	.4byte	.LFE3-.LFB3
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x65b
	.uleb128 0x1b
	.4byte	.LASF623
	.byte	0x1
	.byte	0x3c
	.4byte	0x4a
	.4byte	.LLST15
	.uleb128 0x15
	.4byte	.LVL51
	.4byte	0x6cb
	.4byte	0x651
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x50
	.uleb128 0x2
	.byte	0x8
	.byte	0xc8
	.byte	0
	.uleb128 0x1f
	.4byte	.LVL52
	.4byte	0x676
	.byte	0
	.uleb128 0x22
	.4byte	.LASF663
	.byte	0x1
	.byte	0x39
	.4byte	0x63
	.4byte	0x676
	.uleb128 0x23
	.4byte	.LASF623
	.byte	0x1
	.byte	0x39
	.4byte	0x4a
	.byte	0
	.uleb128 0x1a
	.4byte	.LASF655
	.byte	0x1
	.byte	0x31
	.4byte	.LFB1
	.4byte	.LFE1-.LFB1
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x69e
	.uleb128 0x1f
	.4byte	.LVL38
	.4byte	0x69e
	.uleb128 0x1f
	.4byte	.LVL39
	.4byte	0x6d7
	.byte	0
	.uleb128 0x24
	.4byte	.LASF664
	.byte	0x1
	.byte	0x2b
	.4byte	0x2c
	.4byte	.LFB0
	.4byte	.LFE0-.LFB0
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0x25
	.4byte	.LASF656
	.4byte	.LASF656
	.byte	0x4
	.2byte	0x133
	.uleb128 0x25
	.4byte	.LASF657
	.4byte	.LASF657
	.byte	0x4
	.2byte	0x132
	.uleb128 0x25
	.4byte	.LASF658
	.4byte	.LASF658
	.byte	0x4
	.2byte	0x134
	.uleb128 0x25
	.4byte	.LASF659
	.4byte	.LASF659
	.byte	0x4
	.2byte	0x131
	.byte	0
	.section	.debug_abbrev,"",%progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x55
	.uleb128 0x17
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x17
	.uleb128 0x2119
	.uleb128 0x17
	.uleb128 0x2134
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x35
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x6e
	.uleb128 0xe
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0x15
	.byte	0x1
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0x15
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x34
	.byte	0
	.uleb128 0x47
	.uleb128 0x13
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0x4109
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x13
	.uleb128 0x410a
	.byte	0
	.uleb128 0x2
	.uleb128 0x18
	.uleb128 0x2111
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x14
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x15
	.uleb128 0x4109
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x16
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x17
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x18
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x19
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1c
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x1a
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1b
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x1c
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x1d
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x1e
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1f
	.uleb128 0x4109
	.byte	0
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x20
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1c
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x21
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x22
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x23
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x24
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x25
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x6e
	.uleb128 0xe
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_loc,"",%progbits
.Ldebug_loc0:
.LLST24:
	.4byte	.LVL83
	.4byte	.LVL85
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL85
	.4byte	.LFE14
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x50
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST25:
	.4byte	.LVL83
	.4byte	.LVL84
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL84
	.4byte	.LFE14
	.2byte	0x1
	.byte	0x54
	.4byte	0
	.4byte	0
.LLST16:
	.4byte	.LVL53
	.4byte	.LVL77
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL77
	.4byte	.LFE13
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x50
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST17:
	.4byte	.LVL53
	.4byte	.LVL55
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL55
	.4byte	.LVL56
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x51
	.byte	0x9f
	.4byte	.LVL56
	.4byte	.LVL57
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL57
	.4byte	.LVL58
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x51
	.byte	0x9f
	.4byte	.LVL58
	.4byte	.LVL59
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL59
	.4byte	.LVL60
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x51
	.byte	0x9f
	.4byte	.LVL60
	.4byte	.LVL61
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL61
	.4byte	.LVL62
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x51
	.byte	0x9f
	.4byte	.LVL62
	.4byte	.LVL63
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL63
	.4byte	.LVL64
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x51
	.byte	0x9f
	.4byte	.LVL64
	.4byte	.LVL65
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL65
	.4byte	.LVL66
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x51
	.byte	0x9f
	.4byte	.LVL66
	.4byte	.LVL67
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL67
	.4byte	.LVL68
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x51
	.byte	0x9f
	.4byte	.LVL68
	.4byte	.LVL69
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL69
	.4byte	.LVL70
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x51
	.byte	0x9f
	.4byte	.LVL70
	.4byte	.LVL71
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL71
	.4byte	.LVL72
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x51
	.byte	0x9f
	.4byte	.LVL72
	.4byte	.LVL73
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL73
	.4byte	.LVL74
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x51
	.byte	0x9f
	.4byte	.LVL74
	.4byte	.LVL75
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL75
	.4byte	.LFE13
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x51
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST18:
	.4byte	.LVL53
	.4byte	.LVL78-1
	.2byte	0x1
	.byte	0x52
	.4byte	.LVL78-1
	.4byte	.LVL81
	.2byte	0x1
	.byte	0x56
	.4byte	.LVL81
	.4byte	.LFE13
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x52
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST19:
	.4byte	.LVL53
	.4byte	.LVL78-1
	.2byte	0x1
	.byte	0x53
	.4byte	.LVL78-1
	.4byte	.LVL81
	.2byte	0x1
	.byte	0x55
	.4byte	.LVL81
	.4byte	.LFE13
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x53
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST20:
	.4byte	.LVL53
	.4byte	.LVL82
	.2byte	0x2
	.byte	0x91
	.sleb128 0
	.4byte	.LVL82
	.4byte	.LFE13
	.2byte	0x2
	.byte	0x7d
	.sleb128 0
	.4byte	0
	.4byte	0
.LLST21:
	.4byte	.LVL54
	.4byte	.LVL56
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL56
	.4byte	.LVL58
	.2byte	0x2
	.byte	0x39
	.byte	0x9f
	.4byte	.LVL58
	.4byte	.LVL60
	.2byte	0x2
	.byte	0x3f
	.byte	0x9f
	.4byte	.LVL60
	.4byte	.LVL62
	.2byte	0x2
	.byte	0x45
	.byte	0x9f
	.4byte	.LVL62
	.4byte	.LVL64
	.2byte	0x2
	.byte	0x4b
	.byte	0x9f
	.4byte	.LVL64
	.4byte	.LVL66
	.2byte	0x3
	.byte	0x8
	.byte	0x21
	.byte	0x9f
	.4byte	.LVL66
	.4byte	.LVL68
	.2byte	0x3
	.byte	0x8
	.byte	0x27
	.byte	0x9f
	.4byte	.LVL68
	.4byte	.LVL70
	.2byte	0x3
	.byte	0x8
	.byte	0x2d
	.byte	0x9f
	.4byte	.LVL70
	.4byte	.LVL72
	.2byte	0x3
	.byte	0x8
	.byte	0x33
	.byte	0x9f
	.4byte	.LVL72
	.4byte	.LVL74
	.2byte	0x3
	.byte	0x8
	.byte	0x39
	.byte	0x9f
	.4byte	.LVL74
	.4byte	.LVL75
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL76
	.4byte	.LVL78-1
	.2byte	0x1
	.byte	0x51
	.4byte	0
	.4byte	0
.LLST22:
	.4byte	.LVL54
	.4byte	.LVL56
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL56
	.4byte	.LVL58
	.2byte	0x2
	.byte	0x3d
	.byte	0x9f
	.4byte	.LVL58
	.4byte	.LVL60
	.2byte	0x2
	.byte	0x43
	.byte	0x9f
	.4byte	.LVL60
	.4byte	.LVL62
	.2byte	0x2
	.byte	0x49
	.byte	0x9f
	.4byte	.LVL62
	.4byte	.LVL64
	.2byte	0x2
	.byte	0x4f
	.byte	0x9f
	.4byte	.LVL64
	.4byte	.LVL66
	.2byte	0x3
	.byte	0x8
	.byte	0x25
	.byte	0x9f
	.4byte	.LVL66
	.4byte	.LVL68
	.2byte	0x3
	.byte	0x8
	.byte	0x2b
	.byte	0x9f
	.4byte	.LVL68
	.4byte	.LVL70
	.2byte	0x3
	.byte	0x8
	.byte	0x31
	.byte	0x9f
	.4byte	.LVL70
	.4byte	.LVL72
	.2byte	0x3
	.byte	0x8
	.byte	0x37
	.byte	0x9f
	.4byte	.LVL72
	.4byte	.LVL74
	.2byte	0x3
	.byte	0x8
	.byte	0x3d
	.byte	0x9f
	.4byte	.LVL74
	.4byte	.LVL75
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL76
	.4byte	.LVL81
	.2byte	0x1
	.byte	0x58
	.4byte	0
	.4byte	0
.LLST23:
	.4byte	.LVL54
	.4byte	.LVL56
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL56
	.4byte	.LVL58
	.2byte	0x2
	.byte	0x3e
	.byte	0x9f
	.4byte	.LVL58
	.4byte	.LVL60
	.2byte	0x2
	.byte	0x44
	.byte	0x9f
	.4byte	.LVL60
	.4byte	.LVL62
	.2byte	0x2
	.byte	0x4a
	.byte	0x9f
	.4byte	.LVL62
	.4byte	.LVL64
	.2byte	0x3
	.byte	0x8
	.byte	0x20
	.byte	0x9f
	.4byte	.LVL64
	.4byte	.LVL66
	.2byte	0x3
	.byte	0x8
	.byte	0x26
	.byte	0x9f
	.4byte	.LVL66
	.4byte	.LVL68
	.2byte	0x3
	.byte	0x8
	.byte	0x2c
	.byte	0x9f
	.4byte	.LVL68
	.4byte	.LVL70
	.2byte	0x3
	.byte	0x8
	.byte	0x32
	.byte	0x9f
	.4byte	.LVL70
	.4byte	.LVL72
	.2byte	0x3
	.byte	0x8
	.byte	0x38
	.byte	0x9f
	.4byte	.LVL72
	.4byte	.LVL74
	.2byte	0x3
	.byte	0x8
	.byte	0x3e
	.byte	0x9f
	.4byte	.LVL74
	.4byte	.LVL75
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL76
	.4byte	.LVL81
	.2byte	0x1
	.byte	0x57
	.4byte	0
	.4byte	0
.LLST0:
	.4byte	.LVL2
	.4byte	.LVL3
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL3
	.4byte	.LFE12
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x50
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST11:
	.4byte	.LVL34
	.4byte	.LVL36
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL36
	.4byte	.LFE10
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x50
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST1:
	.4byte	.LVL4
	.4byte	.LVL19
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL19
	.4byte	.LFE9
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x50
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST2:
	.4byte	.LVL4
	.4byte	.LVL18
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL18
	.4byte	.LVL24
	.2byte	0x1
	.byte	0x57
	.4byte	.LVL24
	.4byte	.LFE9
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x51
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST3:
	.4byte	.LVL4
	.4byte	.LVL20-1
	.2byte	0x1
	.byte	0x52
	.4byte	.LVL20-1
	.4byte	.LVL24
	.2byte	0x1
	.byte	0x55
	.4byte	.LVL24
	.4byte	.LFE9
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x52
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST4:
	.4byte	.LVL5
	.4byte	.LVL6
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL6
	.4byte	.LVL7
	.2byte	0x2
	.byte	0x3a
	.byte	0x9f
	.4byte	.LVL7
	.4byte	.LVL8
	.2byte	0x2
	.byte	0x40
	.byte	0x9f
	.4byte	.LVL8
	.4byte	.LVL9
	.2byte	0x2
	.byte	0x46
	.byte	0x9f
	.4byte	.LVL9
	.4byte	.LVL10
	.2byte	0x2
	.byte	0x4c
	.byte	0x9f
	.4byte	.LVL10
	.4byte	.LVL11
	.2byte	0x3
	.byte	0x8
	.byte	0x22
	.byte	0x9f
	.4byte	.LVL11
	.4byte	.LVL12
	.2byte	0x3
	.byte	0x8
	.byte	0x28
	.byte	0x9f
	.4byte	.LVL12
	.4byte	.LVL13
	.2byte	0x3
	.byte	0x8
	.byte	0x2e
	.byte	0x9f
	.4byte	.LVL13
	.4byte	.LVL14
	.2byte	0x3
	.byte	0x8
	.byte	0x34
	.byte	0x9f
	.4byte	.LVL14
	.4byte	.LVL15
	.2byte	0x3
	.byte	0x8
	.byte	0x3a
	.byte	0x9f
	.4byte	.LVL15
	.4byte	.LVL16
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL17
	.4byte	.LVL20-1
	.2byte	0x1
	.byte	0x53
	.4byte	0
	.4byte	0
.LLST5:
	.4byte	.LVL5
	.4byte	.LVL6
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL6
	.4byte	.LVL7
	.2byte	0x2
	.byte	0x39
	.byte	0x9f
	.4byte	.LVL7
	.4byte	.LVL8
	.2byte	0x2
	.byte	0x3f
	.byte	0x9f
	.4byte	.LVL8
	.4byte	.LVL9
	.2byte	0x2
	.byte	0x45
	.byte	0x9f
	.4byte	.LVL9
	.4byte	.LVL10
	.2byte	0x2
	.byte	0x4b
	.byte	0x9f
	.4byte	.LVL10
	.4byte	.LVL11
	.2byte	0x3
	.byte	0x8
	.byte	0x21
	.byte	0x9f
	.4byte	.LVL11
	.4byte	.LVL12
	.2byte	0x3
	.byte	0x8
	.byte	0x27
	.byte	0x9f
	.4byte	.LVL12
	.4byte	.LVL13
	.2byte	0x3
	.byte	0x8
	.byte	0x2d
	.byte	0x9f
	.4byte	.LVL13
	.4byte	.LVL14
	.2byte	0x3
	.byte	0x8
	.byte	0x33
	.byte	0x9f
	.4byte	.LVL14
	.4byte	.LVL15
	.2byte	0x3
	.byte	0x8
	.byte	0x39
	.byte	0x9f
	.4byte	.LVL15
	.4byte	.LVL16
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL17
	.4byte	.LVL24
	.2byte	0x1
	.byte	0x5a
	.4byte	0
	.4byte	0
.LLST6:
	.4byte	.LVL5
	.4byte	.LVL6
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL6
	.4byte	.LVL7
	.2byte	0x2
	.byte	0x3c
	.byte	0x9f
	.4byte	.LVL7
	.4byte	.LVL8
	.2byte	0x2
	.byte	0x42
	.byte	0x9f
	.4byte	.LVL8
	.4byte	.LVL9
	.2byte	0x2
	.byte	0x48
	.byte	0x9f
	.4byte	.LVL9
	.4byte	.LVL10
	.2byte	0x2
	.byte	0x4e
	.byte	0x9f
	.4byte	.LVL10
	.4byte	.LVL11
	.2byte	0x3
	.byte	0x8
	.byte	0x24
	.byte	0x9f
	.4byte	.LVL11
	.4byte	.LVL12
	.2byte	0x3
	.byte	0x8
	.byte	0x2a
	.byte	0x9f
	.4byte	.LVL12
	.4byte	.LVL13
	.2byte	0x3
	.byte	0x8
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL13
	.4byte	.LVL14
	.2byte	0x3
	.byte	0x8
	.byte	0x36
	.byte	0x9f
	.4byte	.LVL14
	.4byte	.LVL15
	.2byte	0x3
	.byte	0x8
	.byte	0x3c
	.byte	0x9f
	.4byte	.LVL15
	.4byte	.LVL16
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL17
	.4byte	.LVL24
	.2byte	0x1
	.byte	0x59
	.4byte	0
	.4byte	0
.LLST7:
	.4byte	.LVL5
	.4byte	.LVL6
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL6
	.4byte	.LVL7
	.2byte	0x2
	.byte	0x3b
	.byte	0x9f
	.4byte	.LVL7
	.4byte	.LVL8
	.2byte	0x2
	.byte	0x41
	.byte	0x9f
	.4byte	.LVL8
	.4byte	.LVL9
	.2byte	0x2
	.byte	0x47
	.byte	0x9f
	.4byte	.LVL9
	.4byte	.LVL10
	.2byte	0x2
	.byte	0x4d
	.byte	0x9f
	.4byte	.LVL10
	.4byte	.LVL11
	.2byte	0x3
	.byte	0x8
	.byte	0x23
	.byte	0x9f
	.4byte	.LVL11
	.4byte	.LVL12
	.2byte	0x3
	.byte	0x8
	.byte	0x29
	.byte	0x9f
	.4byte	.LVL12
	.4byte	.LVL13
	.2byte	0x3
	.byte	0x8
	.byte	0x2f
	.byte	0x9f
	.4byte	.LVL13
	.4byte	.LVL14
	.2byte	0x3
	.byte	0x8
	.byte	0x35
	.byte	0x9f
	.4byte	.LVL14
	.4byte	.LVL15
	.2byte	0x3
	.byte	0x8
	.byte	0x3b
	.byte	0x9f
	.4byte	.LVL15
	.4byte	.LVL16
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL17
	.4byte	.LVL24
	.2byte	0x1
	.byte	0x58
	.4byte	0
	.4byte	0
.LLST8:
	.4byte	.LVL25
	.4byte	.LVL26
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL26
	.4byte	.LFE8
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x50
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST10:
	.4byte	.LVL32
	.4byte	.LVL33-1
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL33-1
	.4byte	.LFE7
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x50
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST9:
	.4byte	.LVL28
	.4byte	.LVL30
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL30
	.4byte	.LFE6
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x50
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST12:
	.4byte	.LVL40
	.4byte	.LVL42-1
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL42-1
	.4byte	.LFE4
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x50
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST13:
	.4byte	.LVL41
	.4byte	.LVL43
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL43
	.4byte	.LVL44
	.2byte	0x1
	.byte	0x54
	.4byte	.LVL45
	.4byte	.LFE4
	.2byte	0x1
	.byte	0x54
	.4byte	0
	.4byte	0
.LLST14:
	.4byte	.LVL41
	.4byte	.LVL43
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL43
	.4byte	.LVL47
	.2byte	0x1
	.byte	0x55
	.4byte	.LVL48
	.4byte	.LFE4
	.2byte	0x1
	.byte	0x55
	.4byte	0
	.4byte	0
.LLST15:
	.4byte	.LVL49
	.4byte	.LVL50
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL50
	.4byte	.LFE3
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x50
	.byte	0x9f
	.4byte	0
	.4byte	0
	.section	.debug_pubnames,"",%progbits
	.4byte	0x134
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x6e4
	.4byte	0x1b7
	.ascii	"gTsHandle\000"
	.4byte	0x1c8
	.ascii	"ft5336_ts_drv\000"
	.4byte	0x1d6
	.ascii	"ft5336_TS_GetGestureID\000"
	.4byte	0x238
	.ascii	"ft5336_TS_GetTouchInfo\000"
	.4byte	0x322
	.ascii	"tsITStatus\000"
	.4byte	0x34d
	.ascii	"tsClearIT\000"
	.4byte	0x372
	.ascii	"tsEnableIT\000"
	.4byte	0x3c2
	.ascii	"tsGetXY\000"
	.4byte	0x4ba
	.ascii	"tsDetectTouch\000"
	.4byte	0x507
	.ascii	"tsStart\000"
	.4byte	0x535
	.ascii	"tsDisableIT\000"
	.4byte	0x582
	.ascii	"reset\000"
	.4byte	0x5a5
	.ascii	"readId\000"
	.4byte	0x619
	.ascii	"init\000"
	.4byte	0x65b
	.ascii	"TS_Configure\000"
	.4byte	0x676
	.ascii	"I2C_InitializeIfRequired\000"
	.4byte	0x69e
	.ascii	"Get_I2C_InitializedStatus\000"
	.4byte	0
	.section	.debug_pubtypes,"",%progbits
	.4byte	0xed
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x6e4
	.4byte	0x25
	.ascii	"signed char\000"
	.4byte	0x3c
	.ascii	"unsigned char\000"
	.4byte	0x2c
	.ascii	"uint8_t\000"
	.4byte	0x43
	.ascii	"short int\000"
	.4byte	0x55
	.ascii	"short unsigned int\000"
	.4byte	0x4a
	.ascii	"uint16_t\000"
	.4byte	0x5c
	.ascii	"int\000"
	.4byte	0x6e
	.ascii	"unsigned int\000"
	.4byte	0x63
	.ascii	"uint32_t\000"
	.4byte	0x75
	.ascii	"long long int\000"
	.4byte	0x7c
	.ascii	"long long unsigned int\000"
	.4byte	0x164
	.ascii	"TS_DrvTypeDef\000"
	.4byte	0x1a0
	.ascii	"ft5336_handle_TypeDef\000"
	.4byte	0
	.section	.debug_aranges,"",%progbits
	.4byte	0x84
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x4
	.byte	0
	.2byte	0
	.2byte	0
	.4byte	.LFB0
	.4byte	.LFE0-.LFB0
	.4byte	.LFB5
	.4byte	.LFE5-.LFB5
	.4byte	.LFB11
	.4byte	.LFE11-.LFB11
	.4byte	.LFB12
	.4byte	.LFE12-.LFB12
	.4byte	.LFB9
	.4byte	.LFE9-.LFB9
	.4byte	.LFB8
	.4byte	.LFE8-.LFB8
	.4byte	.LFB6
	.4byte	.LFE6-.LFB6
	.4byte	.LFB7
	.4byte	.LFE7-.LFB7
	.4byte	.LFB10
	.4byte	.LFE10-.LFB10
	.4byte	.LFB1
	.4byte	.LFE1-.LFB1
	.4byte	.LFB4
	.4byte	.LFE4-.LFB4
	.4byte	.LFB3
	.4byte	.LFE3-.LFB3
	.4byte	.LFB13
	.4byte	.LFE13-.LFB13
	.4byte	.LFB14
	.4byte	.LFE14-.LFB14
	.4byte	0
	.4byte	0
	.section	.debug_ranges,"",%progbits
.Ldebug_ranges0:
	.4byte	.LFB0
	.4byte	.LFE0
	.4byte	.LFB5
	.4byte	.LFE5
	.4byte	.LFB11
	.4byte	.LFE11
	.4byte	.LFB12
	.4byte	.LFE12
	.4byte	.LFB9
	.4byte	.LFE9
	.4byte	.LFB8
	.4byte	.LFE8
	.4byte	.LFB6
	.4byte	.LFE6
	.4byte	.LFB7
	.4byte	.LFE7
	.4byte	.LFB10
	.4byte	.LFE10
	.4byte	.LFB1
	.4byte	.LFE1
	.4byte	.LFB4
	.4byte	.LFE4
	.4byte	.LFB3
	.4byte	.LFE3
	.4byte	.LFB13
	.4byte	.LFE13
	.4byte	.LFB14
	.4byte	.LFE14
	.4byte	0
	.4byte	0
	.section	.debug_macro,"",%progbits
.Ldebug_macro0:
	.2byte	0x4
	.byte	0x2
	.4byte	.Ldebug_line0
	.byte	0x7
	.4byte	.Ldebug_macro1
	.byte	0x3
	.uleb128 0
	.uleb128 0x1
	.byte	0x3
	.uleb128 0x26
	.uleb128 0x4
	.byte	0x3
	.uleb128 0x2c
	.uleb128 0x3
	.byte	0x5
	.uleb128 0x28
	.4byte	.LASF402
	.byte	0x3
	.uleb128 0x2f
	.uleb128 0x2
	.byte	0x7
	.4byte	.Ldebug_macro2
	.byte	0x4
	.byte	0x4
	.byte	0x7
	.4byte	.Ldebug_macro3
	.byte	0x4
	.byte	0x4
	.byte	0
	.section	.debug_macro,"G",%progbits,wm4.0.942161b51edd98f0384e0f7b2a0b6149,comdat
.Ldebug_macro1:
	.2byte	0x4
	.byte	0
	.byte	0x5
	.uleb128 0
	.4byte	.LASF0
	.byte	0x5
	.uleb128 0
	.4byte	.LASF1
	.byte	0x5
	.uleb128 0
	.4byte	.LASF2
	.byte	0x5
	.uleb128 0
	.4byte	.LASF3
	.byte	0x5
	.uleb128 0
	.4byte	.LASF4
	.byte	0x5
	.uleb128 0
	.4byte	.LASF5
	.byte	0x5
	.uleb128 0
	.4byte	.LASF6
	.byte	0x5
	.uleb128 0
	.4byte	.LASF7
	.byte	0x5
	.uleb128 0
	.4byte	.LASF8
	.byte	0x5
	.uleb128 0
	.4byte	.LASF9
	.byte	0x5
	.uleb128 0
	.4byte	.LASF10
	.byte	0x5
	.uleb128 0
	.4byte	.LASF11
	.byte	0x5
	.uleb128 0
	.4byte	.LASF12
	.byte	0x5
	.uleb128 0
	.4byte	.LASF13
	.byte	0x5
	.uleb128 0
	.4byte	.LASF14
	.byte	0x5
	.uleb128 0
	.4byte	.LASF15
	.byte	0x5
	.uleb128 0
	.4byte	.LASF16
	.byte	0x5
	.uleb128 0
	.4byte	.LASF17
	.byte	0x5
	.uleb128 0
	.4byte	.LASF18
	.byte	0x5
	.uleb128 0
	.4byte	.LASF19
	.byte	0x5
	.uleb128 0
	.4byte	.LASF20
	.byte	0x5
	.uleb128 0
	.4byte	.LASF21
	.byte	0x5
	.uleb128 0
	.4byte	.LASF22
	.byte	0x5
	.uleb128 0
	.4byte	.LASF23
	.byte	0x5
	.uleb128 0
	.4byte	.LASF24
	.byte	0x5
	.uleb128 0
	.4byte	.LASF25
	.byte	0x5
	.uleb128 0
	.4byte	.LASF26
	.byte	0x5
	.uleb128 0
	.4byte	.LASF27
	.byte	0x5
	.uleb128 0
	.4byte	.LASF28
	.byte	0x5
	.uleb128 0
	.4byte	.LASF29
	.byte	0x5
	.uleb128 0
	.4byte	.LASF30
	.byte	0x5
	.uleb128 0
	.4byte	.LASF31
	.byte	0x5
	.uleb128 0
	.4byte	.LASF32
	.byte	0x5
	.uleb128 0
	.4byte	.LASF33
	.byte	0x5
	.uleb128 0
	.4byte	.LASF34
	.byte	0x5
	.uleb128 0
	.4byte	.LASF35
	.byte	0x5
	.uleb128 0
	.4byte	.LASF36
	.byte	0x5
	.uleb128 0
	.4byte	.LASF37
	.byte	0x5
	.uleb128 0
	.4byte	.LASF38
	.byte	0x5
	.uleb128 0
	.4byte	.LASF39
	.byte	0x5
	.uleb128 0
	.4byte	.LASF40
	.byte	0x5
	.uleb128 0
	.4byte	.LASF41
	.byte	0x5
	.uleb128 0
	.4byte	.LASF42
	.byte	0x5
	.uleb128 0
	.4byte	.LASF43
	.byte	0x5
	.uleb128 0
	.4byte	.LASF44
	.byte	0x5
	.uleb128 0
	.4byte	.LASF45
	.byte	0x5
	.uleb128 0
	.4byte	.LASF46
	.byte	0x5
	.uleb128 0
	.4byte	.LASF47
	.byte	0x5
	.uleb128 0
	.4byte	.LASF48
	.byte	0x5
	.uleb128 0
	.4byte	.LASF49
	.byte	0x5
	.uleb128 0
	.4byte	.LASF50
	.byte	0x5
	.uleb128 0
	.4byte	.LASF51
	.byte	0x5
	.uleb128 0
	.4byte	.LASF52
	.byte	0x5
	.uleb128 0
	.4byte	.LASF53
	.byte	0x5
	.uleb128 0
	.4byte	.LASF54
	.byte	0x5
	.uleb128 0
	.4byte	.LASF55
	.byte	0x5
	.uleb128 0
	.4byte	.LASF56
	.byte	0x5
	.uleb128 0
	.4byte	.LASF57
	.byte	0x5
	.uleb128 0
	.4byte	.LASF58
	.byte	0x5
	.uleb128 0
	.4byte	.LASF59
	.byte	0x5
	.uleb128 0
	.4byte	.LASF60
	.byte	0x5
	.uleb128 0
	.4byte	.LASF61
	.byte	0x5
	.uleb128 0
	.4byte	.LASF62
	.byte	0x5
	.uleb128 0
	.4byte	.LASF63
	.byte	0x5
	.uleb128 0
	.4byte	.LASF64
	.byte	0x5
	.uleb128 0
	.4byte	.LASF65
	.byte	0x5
	.uleb128 0
	.4byte	.LASF66
	.byte	0x5
	.uleb128 0
	.4byte	.LASF67
	.byte	0x5
	.uleb128 0
	.4byte	.LASF68
	.byte	0x5
	.uleb128 0
	.4byte	.LASF69
	.byte	0x5
	.uleb128 0
	.4byte	.LASF70
	.byte	0x5
	.uleb128 0
	.4byte	.LASF71
	.byte	0x5
	.uleb128 0
	.4byte	.LASF72
	.byte	0x5
	.uleb128 0
	.4byte	.LASF73
	.byte	0x5
	.uleb128 0
	.4byte	.LASF74
	.byte	0x5
	.uleb128 0
	.4byte	.LASF75
	.byte	0x5
	.uleb128 0
	.4byte	.LASF76
	.byte	0x5
	.uleb128 0
	.4byte	.LASF77
	.byte	0x5
	.uleb128 0
	.4byte	.LASF78
	.byte	0x5
	.uleb128 0
	.4byte	.LASF79
	.byte	0x5
	.uleb128 0
	.4byte	.LASF80
	.byte	0x5
	.uleb128 0
	.4byte	.LASF81
	.byte	0x5
	.uleb128 0
	.4byte	.LASF82
	.byte	0x5
	.uleb128 0
	.4byte	.LASF83
	.byte	0x5
	.uleb128 0
	.4byte	.LASF84
	.byte	0x5
	.uleb128 0
	.4byte	.LASF85
	.byte	0x5
	.uleb128 0
	.4byte	.LASF86
	.byte	0x5
	.uleb128 0
	.4byte	.LASF87
	.byte	0x5
	.uleb128 0
	.4byte	.LASF88
	.byte	0x5
	.uleb128 0
	.4byte	.LASF89
	.byte	0x5
	.uleb128 0
	.4byte	.LASF90
	.byte	0x5
	.uleb128 0
	.4byte	.LASF91
	.byte	0x5
	.uleb128 0
	.4byte	.LASF92
	.byte	0x5
	.uleb128 0
	.4byte	.LASF93
	.byte	0x5
	.uleb128 0
	.4byte	.LASF94
	.byte	0x5
	.uleb128 0
	.4byte	.LASF95
	.byte	0x5
	.uleb128 0
	.4byte	.LASF96
	.byte	0x5
	.uleb128 0
	.4byte	.LASF97
	.byte	0x5
	.uleb128 0
	.4byte	.LASF98
	.byte	0x5
	.uleb128 0
	.4byte	.LASF99
	.byte	0x5
	.uleb128 0
	.4byte	.LASF100
	.byte	0x5
	.uleb128 0
	.4byte	.LASF101
	.byte	0x5
	.uleb128 0
	.4byte	.LASF102
	.byte	0x5
	.uleb128 0
	.4byte	.LASF103
	.byte	0x5
	.uleb128 0
	.4byte	.LASF104
	.byte	0x5
	.uleb128 0
	.4byte	.LASF105
	.byte	0x5
	.uleb128 0
	.4byte	.LASF106
	.byte	0x5
	.uleb128 0
	.4byte	.LASF107
	.byte	0x5
	.uleb128 0
	.4byte	.LASF108
	.byte	0x5
	.uleb128 0
	.4byte	.LASF109
	.byte	0x5
	.uleb128 0
	.4byte	.LASF110
	.byte	0x5
	.uleb128 0
	.4byte	.LASF111
	.byte	0x5
	.uleb128 0
	.4byte	.LASF112
	.byte	0x5
	.uleb128 0
	.4byte	.LASF113
	.byte	0x5
	.uleb128 0
	.4byte	.LASF114
	.byte	0x5
	.uleb128 0
	.4byte	.LASF115
	.byte	0x5
	.uleb128 0
	.4byte	.LASF116
	.byte	0x5
	.uleb128 0
	.4byte	.LASF117
	.byte	0x5
	.uleb128 0
	.4byte	.LASF118
	.byte	0x5
	.uleb128 0
	.4byte	.LASF119
	.byte	0x5
	.uleb128 0
	.4byte	.LASF120
	.byte	0x5
	.uleb128 0
	.4byte	.LASF121
	.byte	0x5
	.uleb128 0
	.4byte	.LASF122
	.byte	0x5
	.uleb128 0
	.4byte	.LASF123
	.byte	0x5
	.uleb128 0
	.4byte	.LASF124
	.byte	0x5
	.uleb128 0
	.4byte	.LASF125
	.byte	0x5
	.uleb128 0
	.4byte	.LASF126
	.byte	0x5
	.uleb128 0
	.4byte	.LASF127
	.byte	0x5
	.uleb128 0
	.4byte	.LASF128
	.byte	0x5
	.uleb128 0
	.4byte	.LASF129
	.byte	0x5
	.uleb128 0
	.4byte	.LASF130
	.byte	0x5
	.uleb128 0
	.4byte	.LASF131
	.byte	0x5
	.uleb128 0
	.4byte	.LASF132
	.byte	0x5
	.uleb128 0
	.4byte	.LASF133
	.byte	0x5
	.uleb128 0
	.4byte	.LASF134
	.byte	0x5
	.uleb128 0
	.4byte	.LASF135
	.byte	0x5
	.uleb128 0
	.4byte	.LASF136
	.byte	0x5
	.uleb128 0
	.4byte	.LASF137
	.byte	0x5
	.uleb128 0
	.4byte	.LASF138
	.byte	0x5
	.uleb128 0
	.4byte	.LASF139
	.byte	0x5
	.uleb128 0
	.4byte	.LASF140
	.byte	0x5
	.uleb128 0
	.4byte	.LASF141
	.byte	0x5
	.uleb128 0
	.4byte	.LASF142
	.byte	0x5
	.uleb128 0
	.4byte	.LASF143
	.byte	0x5
	.uleb128 0
	.4byte	.LASF144
	.byte	0x5
	.uleb128 0
	.4byte	.LASF145
	.byte	0x5
	.uleb128 0
	.4byte	.LASF146
	.byte	0x5
	.uleb128 0
	.4byte	.LASF147
	.byte	0x5
	.uleb128 0
	.4byte	.LASF148
	.byte	0x5
	.uleb128 0
	.4byte	.LASF149
	.byte	0x5
	.uleb128 0
	.4byte	.LASF150
	.byte	0x5
	.uleb128 0
	.4byte	.LASF151
	.byte	0x5
	.uleb128 0
	.4byte	.LASF152
	.byte	0x5
	.uleb128 0
	.4byte	.LASF153
	.byte	0x5
	.uleb128 0
	.4byte	.LASF154
	.byte	0x5
	.uleb128 0
	.4byte	.LASF155
	.byte	0x5
	.uleb128 0
	.4byte	.LASF156
	.byte	0x5
	.uleb128 0
	.4byte	.LASF157
	.byte	0x5
	.uleb128 0
	.4byte	.LASF158
	.byte	0x5
	.uleb128 0
	.4byte	.LASF159
	.byte	0x5
	.uleb128 0
	.4byte	.LASF160
	.byte	0x5
	.uleb128 0
	.4byte	.LASF161
	.byte	0x5
	.uleb128 0
	.4byte	.LASF162
	.byte	0x5
	.uleb128 0
	.4byte	.LASF163
	.byte	0x5
	.uleb128 0
	.4byte	.LASF164
	.byte	0x5
	.uleb128 0
	.4byte	.LASF165
	.byte	0x5
	.uleb128 0
	.4byte	.LASF166
	.byte	0x5
	.uleb128 0
	.4byte	.LASF167
	.byte	0x5
	.uleb128 0
	.4byte	.LASF168
	.byte	0x5
	.uleb128 0
	.4byte	.LASF169
	.byte	0x5
	.uleb128 0
	.4byte	.LASF170
	.byte	0x5
	.uleb128 0
	.4byte	.LASF171
	.byte	0x5
	.uleb128 0
	.4byte	.LASF172
	.byte	0x5
	.uleb128 0
	.4byte	.LASF173
	.byte	0x5
	.uleb128 0
	.4byte	.LASF174
	.byte	0x5
	.uleb128 0
	.4byte	.LASF175
	.byte	0x5
	.uleb128 0
	.4byte	.LASF176
	.byte	0x5
	.uleb128 0
	.4byte	.LASF177
	.byte	0x5
	.uleb128 0
	.4byte	.LASF178
	.byte	0x5
	.uleb128 0
	.4byte	.LASF179
	.byte	0x5
	.uleb128 0
	.4byte	.LASF180
	.byte	0x5
	.uleb128 0
	.4byte	.LASF181
	.byte	0x5
	.uleb128 0
	.4byte	.LASF182
	.byte	0x5
	.uleb128 0
	.4byte	.LASF183
	.byte	0x5
	.uleb128 0
	.4byte	.LASF184
	.byte	0x5
	.uleb128 0
	.4byte	.LASF185
	.byte	0x5
	.uleb128 0
	.4byte	.LASF186
	.byte	0x5
	.uleb128 0
	.4byte	.LASF187
	.byte	0x5
	.uleb128 0
	.4byte	.LASF188
	.byte	0x5
	.uleb128 0
	.4byte	.LASF189
	.byte	0x5
	.uleb128 0
	.4byte	.LASF190
	.byte	0x5
	.uleb128 0
	.4byte	.LASF191
	.byte	0x5
	.uleb128 0
	.4byte	.LASF192
	.byte	0x5
	.uleb128 0
	.4byte	.LASF193
	.byte	0x5
	.uleb128 0
	.4byte	.LASF194
	.byte	0x5
	.uleb128 0
	.4byte	.LASF195
	.byte	0x5
	.uleb128 0
	.4byte	.LASF196
	.byte	0x5
	.uleb128 0
	.4byte	.LASF197
	.byte	0x5
	.uleb128 0
	.4byte	.LASF198
	.byte	0x5
	.uleb128 0
	.4byte	.LASF199
	.byte	0x5
	.uleb128 0
	.4byte	.LASF200
	.byte	0x5
	.uleb128 0
	.4byte	.LASF201
	.byte	0x5
	.uleb128 0
	.4byte	.LASF202
	.byte	0x5
	.uleb128 0
	.4byte	.LASF203
	.byte	0x5
	.uleb128 0
	.4byte	.LASF204
	.byte	0x5
	.uleb128 0
	.4byte	.LASF205
	.byte	0x5
	.uleb128 0
	.4byte	.LASF206
	.byte	0x5
	.uleb128 0
	.4byte	.LASF207
	.byte	0x5
	.uleb128 0
	.4byte	.LASF208
	.byte	0x5
	.uleb128 0
	.4byte	.LASF209
	.byte	0x5
	.uleb128 0
	.4byte	.LASF210
	.byte	0x5
	.uleb128 0
	.4byte	.LASF211
	.byte	0x5
	.uleb128 0
	.4byte	.LASF212
	.byte	0x5
	.uleb128 0
	.4byte	.LASF213
	.byte	0x5
	.uleb128 0
	.4byte	.LASF214
	.byte	0x5
	.uleb128 0
	.4byte	.LASF215
	.byte	0x5
	.uleb128 0
	.4byte	.LASF216
	.byte	0x5
	.uleb128 0
	.4byte	.LASF217
	.byte	0x5
	.uleb128 0
	.4byte	.LASF218
	.byte	0x5
	.uleb128 0
	.4byte	.LASF219
	.byte	0x5
	.uleb128 0
	.4byte	.LASF220
	.byte	0x5
	.uleb128 0
	.4byte	.LASF221
	.byte	0x5
	.uleb128 0
	.4byte	.LASF222
	.byte	0x5
	.uleb128 0
	.4byte	.LASF223
	.byte	0x5
	.uleb128 0
	.4byte	.LASF224
	.byte	0x5
	.uleb128 0
	.4byte	.LASF225
	.byte	0x5
	.uleb128 0
	.4byte	.LASF226
	.byte	0x5
	.uleb128 0
	.4byte	.LASF227
	.byte	0x5
	.uleb128 0
	.4byte	.LASF228
	.byte	0x5
	.uleb128 0
	.4byte	.LASF229
	.byte	0x5
	.uleb128 0
	.4byte	.LASF230
	.byte	0x5
	.uleb128 0
	.4byte	.LASF231
	.byte	0x5
	.uleb128 0
	.4byte	.LASF232
	.byte	0x5
	.uleb128 0
	.4byte	.LASF233
	.byte	0x5
	.uleb128 0
	.4byte	.LASF234
	.byte	0x5
	.uleb128 0
	.4byte	.LASF235
	.byte	0x5
	.uleb128 0
	.4byte	.LASF236
	.byte	0x5
	.uleb128 0
	.4byte	.LASF237
	.byte	0x5
	.uleb128 0
	.4byte	.LASF238
	.byte	0x5
	.uleb128 0
	.4byte	.LASF239
	.byte	0x5
	.uleb128 0
	.4byte	.LASF240
	.byte	0x5
	.uleb128 0
	.4byte	.LASF241
	.byte	0x5
	.uleb128 0
	.4byte	.LASF242
	.byte	0x5
	.uleb128 0
	.4byte	.LASF243
	.byte	0x5
	.uleb128 0
	.4byte	.LASF244
	.byte	0x5
	.uleb128 0
	.4byte	.LASF245
	.byte	0x5
	.uleb128 0
	.4byte	.LASF246
	.byte	0x5
	.uleb128 0
	.4byte	.LASF247
	.byte	0x5
	.uleb128 0
	.4byte	.LASF248
	.byte	0x5
	.uleb128 0
	.4byte	.LASF249
	.byte	0x5
	.uleb128 0
	.4byte	.LASF250
	.byte	0x5
	.uleb128 0
	.4byte	.LASF251
	.byte	0x5
	.uleb128 0
	.4byte	.LASF252
	.byte	0x5
	.uleb128 0
	.4byte	.LASF253
	.byte	0x5
	.uleb128 0
	.4byte	.LASF254
	.byte	0x5
	.uleb128 0
	.4byte	.LASF255
	.byte	0x5
	.uleb128 0
	.4byte	.LASF256
	.byte	0x5
	.uleb128 0
	.4byte	.LASF257
	.byte	0x5
	.uleb128 0
	.4byte	.LASF258
	.byte	0x5
	.uleb128 0
	.4byte	.LASF259
	.byte	0x5
	.uleb128 0
	.4byte	.LASF260
	.byte	0x5
	.uleb128 0
	.4byte	.LASF261
	.byte	0x5
	.uleb128 0
	.4byte	.LASF262
	.byte	0x5
	.uleb128 0
	.4byte	.LASF263
	.byte	0x5
	.uleb128 0
	.4byte	.LASF264
	.byte	0x5
	.uleb128 0
	.4byte	.LASF265
	.byte	0x5
	.uleb128 0
	.4byte	.LASF266
	.byte	0x5
	.uleb128 0
	.4byte	.LASF267
	.byte	0x5
	.uleb128 0
	.4byte	.LASF268
	.byte	0x5
	.uleb128 0
	.4byte	.LASF269
	.byte	0x5
	.uleb128 0
	.4byte	.LASF270
	.byte	0x5
	.uleb128 0
	.4byte	.LASF271
	.byte	0x5
	.uleb128 0
	.4byte	.LASF272
	.byte	0x5
	.uleb128 0
	.4byte	.LASF273
	.byte	0x5
	.uleb128 0
	.4byte	.LASF274
	.byte	0x5
	.uleb128 0
	.4byte	.LASF275
	.byte	0x5
	.uleb128 0
	.4byte	.LASF276
	.byte	0x5
	.uleb128 0
	.4byte	.LASF277
	.byte	0x5
	.uleb128 0
	.4byte	.LASF278
	.byte	0x5
	.uleb128 0
	.4byte	.LASF279
	.byte	0x5
	.uleb128 0
	.4byte	.LASF280
	.byte	0x5
	.uleb128 0
	.4byte	.LASF281
	.byte	0x5
	.uleb128 0
	.4byte	.LASF282
	.byte	0x5
	.uleb128 0
	.4byte	.LASF283
	.byte	0x5
	.uleb128 0
	.4byte	.LASF284
	.byte	0x5
	.uleb128 0
	.4byte	.LASF285
	.byte	0x5
	.uleb128 0
	.4byte	.LASF286
	.byte	0x5
	.uleb128 0
	.4byte	.LASF287
	.byte	0x5
	.uleb128 0
	.4byte	.LASF288
	.byte	0x5
	.uleb128 0
	.4byte	.LASF289
	.byte	0x5
	.uleb128 0
	.4byte	.LASF290
	.byte	0x5
	.uleb128 0
	.4byte	.LASF291
	.byte	0x5
	.uleb128 0
	.4byte	.LASF292
	.byte	0x5
	.uleb128 0
	.4byte	.LASF293
	.byte	0x5
	.uleb128 0
	.4byte	.LASF294
	.byte	0x5
	.uleb128 0
	.4byte	.LASF295
	.byte	0x5
	.uleb128 0
	.4byte	.LASF296
	.byte	0x5
	.uleb128 0
	.4byte	.LASF297
	.byte	0x5
	.uleb128 0
	.4byte	.LASF298
	.byte	0x5
	.uleb128 0
	.4byte	.LASF299
	.byte	0x5
	.uleb128 0
	.4byte	.LASF300
	.byte	0x5
	.uleb128 0
	.4byte	.LASF301
	.byte	0x5
	.uleb128 0
	.4byte	.LASF302
	.byte	0x5
	.uleb128 0
	.4byte	.LASF303
	.byte	0x5
	.uleb128 0
	.4byte	.LASF304
	.byte	0x5
	.uleb128 0
	.4byte	.LASF305
	.byte	0x5
	.uleb128 0
	.4byte	.LASF306
	.byte	0x5
	.uleb128 0
	.4byte	.LASF307
	.byte	0x5
	.uleb128 0
	.4byte	.LASF308
	.byte	0x5
	.uleb128 0
	.4byte	.LASF309
	.byte	0x5
	.uleb128 0
	.4byte	.LASF310
	.byte	0x5
	.uleb128 0
	.4byte	.LASF311
	.byte	0x5
	.uleb128 0
	.4byte	.LASF312
	.byte	0x5
	.uleb128 0
	.4byte	.LASF313
	.byte	0x5
	.uleb128 0
	.4byte	.LASF314
	.byte	0x5
	.uleb128 0
	.4byte	.LASF315
	.byte	0x5
	.uleb128 0
	.4byte	.LASF316
	.byte	0x5
	.uleb128 0
	.4byte	.LASF317
	.byte	0x5
	.uleb128 0
	.4byte	.LASF318
	.byte	0x5
	.uleb128 0
	.4byte	.LASF319
	.byte	0x5
	.uleb128 0
	.4byte	.LASF320
	.byte	0x5
	.uleb128 0
	.4byte	.LASF321
	.byte	0x5
	.uleb128 0
	.4byte	.LASF322
	.byte	0x5
	.uleb128 0
	.4byte	.LASF323
	.byte	0x5
	.uleb128 0
	.4byte	.LASF324
	.byte	0x5
	.uleb128 0
	.4byte	.LASF325
	.byte	0x5
	.uleb128 0
	.4byte	.LASF326
	.byte	0x5
	.uleb128 0
	.4byte	.LASF327
	.byte	0x5
	.uleb128 0
	.4byte	.LASF328
	.byte	0x5
	.uleb128 0
	.4byte	.LASF329
	.byte	0x5
	.uleb128 0
	.4byte	.LASF330
	.byte	0x5
	.uleb128 0
	.4byte	.LASF331
	.byte	0x5
	.uleb128 0
	.4byte	.LASF332
	.byte	0x5
	.uleb128 0
	.4byte	.LASF333
	.byte	0x5
	.uleb128 0
	.4byte	.LASF334
	.byte	0x5
	.uleb128 0
	.4byte	.LASF335
	.byte	0x5
	.uleb128 0
	.4byte	.LASF336
	.byte	0x5
	.uleb128 0
	.4byte	.LASF337
	.byte	0x5
	.uleb128 0
	.4byte	.LASF338
	.byte	0x5
	.uleb128 0
	.4byte	.LASF339
	.byte	0x5
	.uleb128 0
	.4byte	.LASF340
	.byte	0x5
	.uleb128 0
	.4byte	.LASF341
	.byte	0x5
	.uleb128 0
	.4byte	.LASF342
	.byte	0x5
	.uleb128 0
	.4byte	.LASF343
	.byte	0x5
	.uleb128 0
	.4byte	.LASF344
	.byte	0x5
	.uleb128 0
	.4byte	.LASF345
	.byte	0x5
	.uleb128 0
	.4byte	.LASF346
	.byte	0x5
	.uleb128 0
	.4byte	.LASF347
	.byte	0x5
	.uleb128 0
	.4byte	.LASF348
	.byte	0x5
	.uleb128 0
	.4byte	.LASF349
	.byte	0x5
	.uleb128 0
	.4byte	.LASF350
	.byte	0x5
	.uleb128 0
	.4byte	.LASF351
	.byte	0x5
	.uleb128 0
	.4byte	.LASF352
	.byte	0x5
	.uleb128 0
	.4byte	.LASF353
	.byte	0x5
	.uleb128 0
	.4byte	.LASF354
	.byte	0x5
	.uleb128 0
	.4byte	.LASF355
	.byte	0x5
	.uleb128 0
	.4byte	.LASF356
	.byte	0x5
	.uleb128 0
	.4byte	.LASF357
	.byte	0x5
	.uleb128 0
	.4byte	.LASF358
	.byte	0x5
	.uleb128 0
	.4byte	.LASF359
	.byte	0x6
	.uleb128 0
	.4byte	.LASF360
	.byte	0x5
	.uleb128 0
	.4byte	.LASF361
	.byte	0x6
	.uleb128 0
	.4byte	.LASF362
	.byte	0x5
	.uleb128 0
	.4byte	.LASF363
	.byte	0x5
	.uleb128 0
	.4byte	.LASF364
	.byte	0x5
	.uleb128 0
	.4byte	.LASF365
	.byte	0x5
	.uleb128 0
	.4byte	.LASF366
	.byte	0x5
	.uleb128 0
	.4byte	.LASF367
	.byte	0x5
	.uleb128 0
	.4byte	.LASF368
	.byte	0x5
	.uleb128 0
	.4byte	.LASF369
	.byte	0x5
	.uleb128 0
	.4byte	.LASF370
	.byte	0x5
	.uleb128 0
	.4byte	.LASF371
	.byte	0x5
	.uleb128 0
	.4byte	.LASF372
	.byte	0x5
	.uleb128 0
	.4byte	.LASF373
	.byte	0x5
	.uleb128 0
	.4byte	.LASF374
	.byte	0x5
	.uleb128 0
	.4byte	.LASF375
	.byte	0x5
	.uleb128 0
	.4byte	.LASF376
	.byte	0x5
	.uleb128 0
	.4byte	.LASF377
	.byte	0x5
	.uleb128 0
	.4byte	.LASF378
	.byte	0x5
	.uleb128 0
	.4byte	.LASF379
	.byte	0x5
	.uleb128 0
	.4byte	.LASF380
	.byte	0x6
	.uleb128 0
	.4byte	.LASF381
	.byte	0x6
	.uleb128 0
	.4byte	.LASF382
	.byte	0x6
	.uleb128 0
	.4byte	.LASF383
	.byte	0x5
	.uleb128 0
	.4byte	.LASF384
	.byte	0x5
	.uleb128 0
	.4byte	.LASF385
	.byte	0x5
	.uleb128 0
	.4byte	.LASF386
	.byte	0x5
	.uleb128 0
	.4byte	.LASF387
	.byte	0x5
	.uleb128 0
	.4byte	.LASF388
	.byte	0x5
	.uleb128 0
	.4byte	.LASF389
	.byte	0x5
	.uleb128 0
	.4byte	.LASF390
	.byte	0x5
	.uleb128 0
	.4byte	.LASF391
	.byte	0x5
	.uleb128 0
	.4byte	.LASF392
	.byte	0x5
	.uleb128 0
	.4byte	.LASF393
	.byte	0x5
	.uleb128 0
	.4byte	.LASF394
	.byte	0x5
	.uleb128 0
	.4byte	.LASF385
	.byte	0x5
	.uleb128 0
	.4byte	.LASF395
	.byte	0x5
	.uleb128 0
	.4byte	.LASF396
	.byte	0x5
	.uleb128 0
	.4byte	.LASF397
	.byte	0x5
	.uleb128 0
	.4byte	.LASF398
	.byte	0x5
	.uleb128 0
	.4byte	.LASF399
	.byte	0x5
	.uleb128 0
	.4byte	.LASF400
	.byte	0x5
	.uleb128 0
	.4byte	.LASF401
	.byte	0
	.section	.debug_macro,"G",%progbits,wm4.stdint.h.13.e7ff37f6f039a2e2d3a3b754131cfc10,comdat
.Ldebug_macro2:
	.2byte	0x4
	.byte	0
	.byte	0x5
	.uleb128 0xd
	.4byte	.LASF403
	.byte	0x5
	.uleb128 0x59
	.4byte	.LASF404
	.byte	0x5
	.uleb128 0x5b
	.4byte	.LASF405
	.byte	0x5
	.uleb128 0x5c
	.4byte	.LASF406
	.byte	0x5
	.uleb128 0x5e
	.4byte	.LASF407
	.byte	0x5
	.uleb128 0x60
	.4byte	.LASF408
	.byte	0x5
	.uleb128 0x61
	.4byte	.LASF409
	.byte	0x5
	.uleb128 0x63
	.4byte	.LASF410
	.byte	0x5
	.uleb128 0x64
	.4byte	.LASF411
	.byte	0x5
	.uleb128 0x65
	.4byte	.LASF412
	.byte	0x5
	.uleb128 0x67
	.4byte	.LASF413
	.byte	0x5
	.uleb128 0x68
	.4byte	.LASF414
	.byte	0x5
	.uleb128 0x69
	.4byte	.LASF415
	.byte	0x5
	.uleb128 0x6b
	.4byte	.LASF416
	.byte	0x5
	.uleb128 0x6c
	.4byte	.LASF417
	.byte	0x5
	.uleb128 0x6d
	.4byte	.LASF418
	.byte	0x5
	.uleb128 0x70
	.4byte	.LASF419
	.byte	0x5
	.uleb128 0x71
	.4byte	.LASF420
	.byte	0x5
	.uleb128 0x72
	.4byte	.LASF421
	.byte	0x5
	.uleb128 0x73
	.4byte	.LASF422
	.byte	0x5
	.uleb128 0x74
	.4byte	.LASF423
	.byte	0x5
	.uleb128 0x75
	.4byte	.LASF424
	.byte	0x5
	.uleb128 0x76
	.4byte	.LASF425
	.byte	0x5
	.uleb128 0x77
	.4byte	.LASF426
	.byte	0x5
	.uleb128 0x78
	.4byte	.LASF427
	.byte	0x5
	.uleb128 0x79
	.4byte	.LASF428
	.byte	0x5
	.uleb128 0x7a
	.4byte	.LASF429
	.byte	0x5
	.uleb128 0x7b
	.4byte	.LASF430
	.byte	0x5
	.uleb128 0x7d
	.4byte	.LASF431
	.byte	0x5
	.uleb128 0x7e
	.4byte	.LASF432
	.byte	0x5
	.uleb128 0x7f
	.4byte	.LASF433
	.byte	0x5
	.uleb128 0x80
	.4byte	.LASF434
	.byte	0x5
	.uleb128 0x81
	.4byte	.LASF435
	.byte	0x5
	.uleb128 0x82
	.4byte	.LASF436
	.byte	0x5
	.uleb128 0x83
	.4byte	.LASF437
	.byte	0x5
	.uleb128 0x84
	.4byte	.LASF438
	.byte	0x5
	.uleb128 0x85
	.4byte	.LASF439
	.byte	0x5
	.uleb128 0x86
	.4byte	.LASF440
	.byte	0x5
	.uleb128 0x87
	.4byte	.LASF441
	.byte	0x5
	.uleb128 0x88
	.4byte	.LASF442
	.byte	0x5
	.uleb128 0x8d
	.4byte	.LASF443
	.byte	0x5
	.uleb128 0x8e
	.4byte	.LASF444
	.byte	0x5
	.uleb128 0x8f
	.4byte	.LASF445
	.byte	0x5
	.uleb128 0x91
	.4byte	.LASF446
	.byte	0x5
	.uleb128 0x92
	.4byte	.LASF447
	.byte	0x5
	.uleb128 0x93
	.4byte	.LASF448
	.byte	0x5
	.uleb128 0xa3
	.4byte	.LASF449
	.byte	0x5
	.uleb128 0xa4
	.4byte	.LASF450
	.byte	0x5
	.uleb128 0xa5
	.4byte	.LASF451
	.byte	0x5
	.uleb128 0xa6
	.4byte	.LASF452
	.byte	0x5
	.uleb128 0xa7
	.4byte	.LASF453
	.byte	0x5
	.uleb128 0xa8
	.4byte	.LASF454
	.byte	0x5
	.uleb128 0xa9
	.4byte	.LASF455
	.byte	0x5
	.uleb128 0xaa
	.4byte	.LASF456
	.byte	0x5
	.uleb128 0xac
	.4byte	.LASF457
	.byte	0x5
	.uleb128 0xad
	.4byte	.LASF458
	.byte	0x5
	.uleb128 0xb7
	.4byte	.LASF459
	.byte	0x5
	.uleb128 0xb8
	.4byte	.LASF460
	.byte	0x5
	.uleb128 0xbc
	.4byte	.LASF461
	.byte	0x5
	.uleb128 0xbd
	.4byte	.LASF462
	.byte	0
	.section	.debug_macro,"G",%progbits,wm4.ft5336.h.49.d262d4f579c86f06619617e016b25736,comdat
.Ldebug_macro3:
	.2byte	0x4
	.byte	0
	.byte	0x5
	.uleb128 0x31
	.4byte	.LASF463
	.byte	0x5
	.uleb128 0x43
	.4byte	.LASF464
	.byte	0x5
	.uleb128 0x46
	.4byte	.LASF465
	.byte	0x5
	.uleb128 0x47
	.4byte	.LASF466
	.byte	0x5
	.uleb128 0x4a
	.4byte	.LASF467
	.byte	0x5
	.uleb128 0x4b
	.4byte	.LASF468
	.byte	0x5
	.uleb128 0x4e
	.4byte	.LASF469
	.byte	0x5
	.uleb128 0x4f
	.4byte	.LASF470
	.byte	0x5
	.uleb128 0x52
	.4byte	.LASF471
	.byte	0x5
	.uleb128 0x55
	.4byte	.LASF472
	.byte	0x5
	.uleb128 0x58
	.4byte	.LASF473
	.byte	0x5
	.uleb128 0x59
	.4byte	.LASF474
	.byte	0x5
	.uleb128 0x5b
	.4byte	.LASF475
	.byte	0x5
	.uleb128 0x5c
	.4byte	.LASF476
	.byte	0x5
	.uleb128 0x5f
	.4byte	.LASF477
	.byte	0x5
	.uleb128 0x62
	.4byte	.LASF478
	.byte	0x5
	.uleb128 0x63
	.4byte	.LASF479
	.byte	0x5
	.uleb128 0x64
	.4byte	.LASF480
	.byte	0x5
	.uleb128 0x65
	.4byte	.LASF481
	.byte	0x5
	.uleb128 0x66
	.4byte	.LASF482
	.byte	0x5
	.uleb128 0x67
	.4byte	.LASF483
	.byte	0x5
	.uleb128 0x68
	.4byte	.LASF484
	.byte	0x5
	.uleb128 0x69
	.4byte	.LASF485
	.byte	0x5
	.uleb128 0x6a
	.4byte	.LASF486
	.byte	0x5
	.uleb128 0x6b
	.4byte	.LASF487
	.byte	0x5
	.uleb128 0x6c
	.4byte	.LASF488
	.byte	0x5
	.uleb128 0x6f
	.4byte	.LASF489
	.byte	0x5
	.uleb128 0x72
	.4byte	.LASF490
	.byte	0x5
	.uleb128 0x73
	.4byte	.LASF491
	.byte	0x5
	.uleb128 0x76
	.4byte	.LASF492
	.byte	0x5
	.uleb128 0x77
	.4byte	.LASF493
	.byte	0x5
	.uleb128 0x78
	.4byte	.LASF494
	.byte	0x5
	.uleb128 0x79
	.4byte	.LASF495
	.byte	0x5
	.uleb128 0x7b
	.4byte	.LASF496
	.byte	0x5
	.uleb128 0x7c
	.4byte	.LASF497
	.byte	0x5
	.uleb128 0x7e
	.4byte	.LASF498
	.byte	0x5
	.uleb128 0x7f
	.4byte	.LASF499
	.byte	0x5
	.uleb128 0x82
	.4byte	.LASF500
	.byte	0x5
	.uleb128 0x83
	.4byte	.LASF501
	.byte	0x5
	.uleb128 0x85
	.4byte	.LASF502
	.byte	0x5
	.uleb128 0x86
	.4byte	.LASF503
	.byte	0x5
	.uleb128 0x87
	.4byte	.LASF504
	.byte	0x5
	.uleb128 0x88
	.4byte	.LASF505
	.byte	0x5
	.uleb128 0x8b
	.4byte	.LASF506
	.byte	0x5
	.uleb128 0x8e
	.4byte	.LASF507
	.byte	0x5
	.uleb128 0x8f
	.4byte	.LASF508
	.byte	0x5
	.uleb128 0x92
	.4byte	.LASF509
	.byte	0x5
	.uleb128 0x95
	.4byte	.LASF510
	.byte	0x5
	.uleb128 0x96
	.4byte	.LASF511
	.byte	0x5
	.uleb128 0x98
	.4byte	.LASF512
	.byte	0x5
	.uleb128 0x99
	.4byte	.LASF513
	.byte	0x5
	.uleb128 0x9a
	.4byte	.LASF514
	.byte	0x5
	.uleb128 0x9b
	.4byte	.LASF515
	.byte	0x5
	.uleb128 0x9c
	.4byte	.LASF516
	.byte	0x5
	.uleb128 0x9d
	.4byte	.LASF517
	.byte	0x5
	.uleb128 0x9f
	.4byte	.LASF518
	.byte	0x5
	.uleb128 0xa0
	.4byte	.LASF519
	.byte	0x5
	.uleb128 0xa1
	.4byte	.LASF520
	.byte	0x5
	.uleb128 0xa2
	.4byte	.LASF521
	.byte	0x5
	.uleb128 0xa3
	.4byte	.LASF522
	.byte	0x5
	.uleb128 0xa4
	.4byte	.LASF523
	.byte	0x5
	.uleb128 0xa6
	.4byte	.LASF524
	.byte	0x5
	.uleb128 0xa7
	.4byte	.LASF525
	.byte	0x5
	.uleb128 0xa8
	.4byte	.LASF526
	.byte	0x5
	.uleb128 0xa9
	.4byte	.LASF527
	.byte	0x5
	.uleb128 0xaa
	.4byte	.LASF528
	.byte	0x5
	.uleb128 0xab
	.4byte	.LASF529
	.byte	0x5
	.uleb128 0xad
	.4byte	.LASF530
	.byte	0x5
	.uleb128 0xae
	.4byte	.LASF531
	.byte	0x5
	.uleb128 0xaf
	.4byte	.LASF532
	.byte	0x5
	.uleb128 0xb0
	.4byte	.LASF533
	.byte	0x5
	.uleb128 0xb1
	.4byte	.LASF534
	.byte	0x5
	.uleb128 0xb2
	.4byte	.LASF535
	.byte	0x5
	.uleb128 0xb4
	.4byte	.LASF536
	.byte	0x5
	.uleb128 0xb5
	.4byte	.LASF537
	.byte	0x5
	.uleb128 0xb6
	.4byte	.LASF538
	.byte	0x5
	.uleb128 0xb7
	.4byte	.LASF539
	.byte	0x5
	.uleb128 0xb8
	.4byte	.LASF540
	.byte	0x5
	.uleb128 0xb9
	.4byte	.LASF541
	.byte	0x5
	.uleb128 0xbb
	.4byte	.LASF542
	.byte	0x5
	.uleb128 0xbc
	.4byte	.LASF543
	.byte	0x5
	.uleb128 0xbd
	.4byte	.LASF544
	.byte	0x5
	.uleb128 0xbe
	.4byte	.LASF545
	.byte	0x5
	.uleb128 0xbf
	.4byte	.LASF546
	.byte	0x5
	.uleb128 0xc0
	.4byte	.LASF547
	.byte	0x5
	.uleb128 0xc2
	.4byte	.LASF548
	.byte	0x5
	.uleb128 0xc3
	.4byte	.LASF549
	.byte	0x5
	.uleb128 0xc4
	.4byte	.LASF550
	.byte	0x5
	.uleb128 0xc5
	.4byte	.LASF551
	.byte	0x5
	.uleb128 0xc6
	.4byte	.LASF552
	.byte	0x5
	.uleb128 0xc7
	.4byte	.LASF553
	.byte	0x5
	.uleb128 0xc9
	.4byte	.LASF554
	.byte	0x5
	.uleb128 0xca
	.4byte	.LASF555
	.byte	0x5
	.uleb128 0xcb
	.4byte	.LASF556
	.byte	0x5
	.uleb128 0xcc
	.4byte	.LASF557
	.byte	0x5
	.uleb128 0xcd
	.4byte	.LASF558
	.byte	0x5
	.uleb128 0xce
	.4byte	.LASF559
	.byte	0x5
	.uleb128 0xd0
	.4byte	.LASF560
	.byte	0x5
	.uleb128 0xd1
	.4byte	.LASF561
	.byte	0x5
	.uleb128 0xd2
	.4byte	.LASF562
	.byte	0x5
	.uleb128 0xd3
	.4byte	.LASF563
	.byte	0x5
	.uleb128 0xd4
	.4byte	.LASF564
	.byte	0x5
	.uleb128 0xd5
	.4byte	.LASF565
	.byte	0x5
	.uleb128 0xd8
	.4byte	.LASF566
	.byte	0x5
	.uleb128 0xdb
	.4byte	.LASF567
	.byte	0x5
	.uleb128 0xdc
	.4byte	.LASF568
	.byte	0x5
	.uleb128 0xdf
	.4byte	.LASF569
	.byte	0x5
	.uleb128 0xe2
	.4byte	.LASF570
	.byte	0x5
	.uleb128 0xe7
	.4byte	.LASF571
	.byte	0x5
	.uleb128 0xea
	.4byte	.LASF572
	.byte	0x5
	.uleb128 0xed
	.4byte	.LASF573
	.byte	0x5
	.uleb128 0xf0
	.4byte	.LASF574
	.byte	0x5
	.uleb128 0xf3
	.4byte	.LASF575
	.byte	0x5
	.uleb128 0xf6
	.4byte	.LASF576
	.byte	0x5
	.uleb128 0xf9
	.4byte	.LASF577
	.byte	0x5
	.uleb128 0xfc
	.4byte	.LASF578
	.byte	0x5
	.uleb128 0xff
	.4byte	.LASF579
	.byte	0x5
	.uleb128 0x102
	.4byte	.LASF580
	.byte	0x5
	.uleb128 0x105
	.4byte	.LASF581
	.byte	0x5
	.uleb128 0x108
	.4byte	.LASF582
	.byte	0x5
	.uleb128 0x10b
	.4byte	.LASF583
	.byte	0x5
	.uleb128 0x10e
	.4byte	.LASF584
	.byte	0x5
	.uleb128 0x111
	.4byte	.LASF585
	.byte	0x5
	.uleb128 0x113
	.4byte	.LASF586
	.byte	0x5
	.uleb128 0x114
	.4byte	.LASF587
	.byte	0x5
	.uleb128 0x117
	.4byte	.LASF588
	.byte	0x5
	.uleb128 0x118
	.4byte	.LASF589
	.byte	0x5
	.uleb128 0x11b
	.4byte	.LASF590
	.byte	0x5
	.uleb128 0x11e
	.4byte	.LASF591
	.byte	0x5
	.uleb128 0x121
	.4byte	.LASF592
	.byte	0x5
	.uleb128 0x124
	.4byte	.LASF593
	.byte	0x5
	.uleb128 0x127
	.4byte	.LASF594
	.byte	0x5
	.uleb128 0x12a
	.4byte	.LASF595
	.byte	0
	.section	.debug_line,"",%progbits
.Ldebug_line0:
	.section	.debug_str,"MS",%progbits,1
.LASF608:
	.ascii	"Reset\000"
.LASF574:
	.ascii	"FT5336_PERIODACTIVE_REG ((uint8_t)0x88)\000"
.LASF112:
	.ascii	"__SIG_ATOMIC_MAX__ 0x7fffffff\000"
.LASF66:
	.ascii	"__UINT_FAST64_TYPE__ long long unsigned int\000"
.LASF377:
	.ascii	"__ARMEL__ 1\000"
.LASF133:
	.ascii	"__UINT16_C(c) c\000"
.LASF209:
	.ascii	"__DEC64_SUBNORMAL_MIN__ 0.000000000000001E-383DD\000"
.LASF174:
	.ascii	"__DBL_DECIMAL_DIG__ 17\000"
.LASF416:
	.ascii	"INTMAX_MIN (-9223372036854775807LL-1)\000"
.LASF63:
	.ascii	"__UINT_FAST8_TYPE__ unsigned int\000"
.LASF395:
	.ascii	"__SES_ARM 1\000"
.LASF385:
	.ascii	"__ARM_ARCH_7EM__ 1\000"
.LASF154:
	.ascii	"__FLT_DIG__ 6\000"
.LASF423:
	.ascii	"INT_LEAST8_MAX INT8_MAX\000"
.LASF609:
	.ascii	"Start\000"
.LASF351:
	.ascii	"__GCC_ATOMIC_TEST_AND_SET_TRUEVAL 1\000"
.LASF451:
	.ascii	"INT16_C(x) (x)\000"
.LASF208:
	.ascii	"__DEC64_EPSILON__ 1E-15DD\000"
.LASF286:
	.ascii	"__ULACCUM_EPSILON__ 0x1P-32ULK\000"
.LASF178:
	.ascii	"__DBL_DENORM_MIN__ double(1.1)\000"
.LASF83:
	.ascii	"__cpp_range_based_for 200907\000"
.LASF131:
	.ascii	"__UINT8_C(c) c\000"
.LASF381:
	.ascii	"__ARM_NEON__\000"
.LASF468:
	.ascii	"FT5336_STATUS_NOT_OK ((uint8_t)0x01)\000"
.LASF298:
	.ascii	"__QQ_IBIT__ 0\000"
.LASF314:
	.ascii	"__UDQ_IBIT__ 0\000"
.LASF589:
	.ascii	"FT5336_G_MODE_INTERRUPT_TRIGGER ((uint8_t)0x01)\000"
.LASF102:
	.ascii	"__WCHAR_MAX__ 0xffffffffU\000"
.LASF355:
	.ascii	"__SIZEOF_WINT_T__ 4\000"
.LASF91:
	.ascii	"__cpp_delegating_constructors 200604\000"
.LASF170:
	.ascii	"__DBL_MIN_EXP__ (-1021)\000"
.LASF490:
	.ascii	"FT5336_TD_STAT_MASK ((uint8_t)0x0F)\000"
.LASF70:
	.ascii	"__has_include_next(STR) __has_include_next__(STR)\000"
.LASF368:
	.ascii	"__ARM_SIZEOF_WCHAR_T 4\000"
.LASF480:
	.ascii	"FT5336_GEST_ID_MOVE_RIGHT ((uint8_t)0x14)\000"
.LASF248:
	.ascii	"__LLFRACT_IBIT__ 0\000"
.LASF311:
	.ascii	"__USQ_FBIT__ 32\000"
.LASF553:
	.ascii	"FT5336_P8_MISC_REG ((uint8_t)0x32)\000"
.LASF224:
	.ascii	"__USFRACT_MIN__ 0.0UHR\000"
.LASF590:
	.ascii	"FT5336_PWR_MODE_REG ((uint8_t)0xA5)\000"
.LASF557:
	.ascii	"FT5336_P9_YL_REG ((uint8_t)0x36)\000"
.LASF6:
	.ascii	"__GNUC_MINOR__ 3\000"
.LASF438:
	.ascii	"INT_FAST64_MAX INT64_MAX\000"
.LASF633:
	.ascii	"regAddressXHigh\000"
.LASF619:
	.ascii	"i2cInitialized\000"
.LASF429:
	.ascii	"UINT_LEAST32_MAX UINT32_MAX\000"
.LASF603:
	.ascii	"unsigned int\000"
.LASF86:
	.ascii	"__cpp_attributes 200809\000"
.LASF24:
	.ascii	"__SIZEOF_SIZE_T__ 4\000"
.LASF40:
	.ascii	"__CHAR16_TYPE__ short unsigned int\000"
.LASF508:
	.ascii	"FT5336_TOUCH_WEIGHT_SHIFT ((uint8_t)0x00)\000"
.LASF527:
	.ascii	"FT5336_P4_YL_REG ((uint8_t)0x18)\000"
.LASF23:
	.ascii	"__SIZEOF_LONG_DOUBLE__ 8\000"
.LASF160:
	.ascii	"__FLT_MAX__ 1.1\000"
.LASF138:
	.ascii	"__INT_FAST8_MAX__ 0x7fffffff\000"
.LASF28:
	.ascii	"__ORDER_BIG_ENDIAN__ 4321\000"
.LASF640:
	.ascii	"coord\000"
.LASF638:
	.ascii	"regValue\000"
.LASF213:
	.ascii	"__DEC128_MIN__ 1E-6143DL\000"
.LASF357:
	.ascii	"__ARM_FEATURE_DSP 1\000"
.LASF587:
	.ascii	"FT5336_G_MODE_INTERRUPT_SHIFT ((uint8_t)0x00)\000"
.LASF664:
	.ascii	"Get_I2C_InitializedStatus\000"
.LASF331:
	.ascii	"__UTA_FBIT__ 64\000"
.LASF168:
	.ascii	"__DBL_MANT_DIG__ 53\000"
.LASF58:
	.ascii	"__UINT_LEAST64_TYPE__ long long unsigned int\000"
.LASF99:
	.ascii	"__INT_MAX__ 0x7fffffff\000"
.LASF12:
	.ascii	"__ATOMIC_RELEASE 3\000"
.LASF472:
	.ascii	"FT5336_DEV_MODE_REG ((uint8_t)0x00)\000"
.LASF35:
	.ascii	"__PTRDIFF_TYPE__ int\000"
.LASF337:
	.ascii	"__CHAR_UNSIGNED__ 1\000"
.LASF44:
	.ascii	"__INT16_TYPE__ short int\000"
.LASF362:
	.ascii	"__ARM_FEATURE_QRDMX\000"
.LASF201:
	.ascii	"__DEC32_EPSILON__ 1E-6DF\000"
.LASF183:
	.ascii	"__LDBL_DIG__ 15\000"
.LASF18:
	.ascii	"__SIZEOF_LONG__ 4\000"
.LASF427:
	.ascii	"UINT_LEAST8_MAX UINT8_MAX\000"
.LASF288:
	.ascii	"__LLACCUM_IBIT__ 32\000"
.LASF10:
	.ascii	"__ATOMIC_SEQ_CST 5\000"
.LASF20:
	.ascii	"__SIZEOF_SHORT__ 2\000"
.LASF95:
	.ascii	"__cpp_alias_templates 200704\000"
.LASF396:
	.ascii	"__ARM_ARCH_FPV5_SP_D16__ 1\000"
.LASF191:
	.ascii	"__LDBL_EPSILON__ 1.1\000"
.LASF518:
	.ascii	"FT5336_P3_XH_REG ((uint8_t)0x0F)\000"
.LASF51:
	.ascii	"__INT_LEAST8_TYPE__ signed char\000"
.LASF363:
	.ascii	"__ARM_32BIT_STATE 1\000"
.LASF651:
	.ascii	"ucReadId\000"
.LASF111:
	.ascii	"__UINTMAX_C(c) c ## ULL\000"
.LASF495:
	.ascii	"FT5336_TOUCH_EVT_FLAG_NO_EVENT ((uint8_t)0x03)\000"
.LASF497:
	.ascii	"FT5336_TOUCH_EVT_FLAG_MASK ((uint8_t)(3 << FT5336_T"
	.ascii	"OUCH_EVT_FLAG_SHIFT))\000"
.LASF485:
	.ascii	"FT5336_GEST_ID_ROTATE_CLOCKWISE ((uint8_t)0x28)\000"
.LASF30:
	.ascii	"__BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__\000"
.LASF202:
	.ascii	"__DEC32_SUBNORMAL_MIN__ 0.000001E-95DF\000"
.LASF440:
	.ascii	"UINT_FAST16_MAX UINT32_MAX\000"
.LASF238:
	.ascii	"__LFRACT_IBIT__ 0\000"
.LASF538:
	.ascii	"FT5336_P6_YH_REG ((uint8_t)0x23)\000"
.LASF371:
	.ascii	"__ARM_ARCH 7\000"
.LASF3:
	.ascii	"__STDC_UTF_32__ 1\000"
.LASF85:
	.ascii	"__cpp_decltype 200707\000"
.LASF624:
	.ascii	"pGestureId\000"
.LASF97:
	.ascii	"__SCHAR_MAX__ 0x7f\000"
.LASF566:
	.ascii	"FT5336_TH_GROUP_REG ((uint8_t)0x80)\000"
.LASF216:
	.ascii	"__DEC128_SUBNORMAL_MIN__ 0.000000000000000000000000"
	.ascii	"000000001E-6143DL\000"
.LASF253:
	.ascii	"__ULLFRACT_IBIT__ 0\000"
.LASF641:
	.ascii	"regAddressXLow\000"
.LASF576:
	.ascii	"FT5336_RADIAN_VALUE_REG ((uint8_t)0x91)\000"
.LASF9:
	.ascii	"__ATOMIC_RELAXED 0\000"
.LASF53:
	.ascii	"__INT_LEAST32_TYPE__ long int\000"
.LASF528:
	.ascii	"FT5336_P4_WEIGHT_REG ((uint8_t)0x19)\000"
.LASF107:
	.ascii	"__SIZE_MAX__ 0xffffffffU\000"
.LASF279:
	.ascii	"__LACCUM_MIN__ (-0X1P31LK-0X1P31LK)\000"
.LASF295:
	.ascii	"__ULLACCUM_MAX__ 0XFFFFFFFFFFFFFFFFP-32ULLK\000"
.LASF284:
	.ascii	"__ULACCUM_MIN__ 0.0ULK\000"
.LASF84:
	.ascii	"__cpp_static_assert 200410\000"
.LASF165:
	.ascii	"__FLT_HAS_INFINITY__ 1\000"
.LASF360:
	.ascii	"__ARM_FEATURE_CRYPTO\000"
.LASF78:
	.ascii	"__cpp_raw_strings 200710\000"
.LASF541:
	.ascii	"FT5336_P6_MISC_REG ((uint8_t)0x26)\000"
.LASF330:
	.ascii	"__UDA_IBIT__ 32\000"
.LASF437:
	.ascii	"INT_FAST32_MAX INT32_MAX\000"
.LASF466:
	.ascii	"FT5336_MAX_HEIGHT ((uint16_t)272)\000"
.LASF62:
	.ascii	"__INT_FAST64_TYPE__ long long int\000"
.LASF123:
	.ascii	"__INT8_C(c) c\000"
.LASF374:
	.ascii	"__thumb2__ 1\000"
.LASF596:
	.ascii	"signed char\000"
.LASF441:
	.ascii	"UINT_FAST32_MAX UINT32_MAX\000"
.LASF479:
	.ascii	"FT5336_GEST_ID_MOVE_UP ((uint8_t)0x10)\000"
.LASF499:
	.ascii	"FT5336_TOUCH_POS_MSB_SHIFT ((uint8_t)0x00)\000"
.LASF294:
	.ascii	"__ULLACCUM_MIN__ 0.0ULLK\000"
.LASF602:
	.ascii	"uint32_t\000"
.LASF657:
	.ascii	"TS_IO_Write\000"
.LASF382:
	.ascii	"__ARM_NEON\000"
.LASF610:
	.ascii	"DetectTouch\000"
.LASF511:
	.ascii	"FT5336_TOUCH_AREA_SHIFT ((uint8_t)0x04)\000"
.LASF652:
	.ascii	"nbReadAttempts\000"
.LASF162:
	.ascii	"__FLT_EPSILON__ 1.1\000"
.LASF506:
	.ascii	"FT5336_P1_WEIGHT_REG ((uint8_t)0x07)\000"
.LASF581:
	.ascii	"FT5336_DISTANCE_ZOOM_REG ((uint8_t)0x96)\000"
.LASF339:
	.ascii	"__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1 1\000"
.LASF453:
	.ascii	"INT32_C(x) (x ##L)\000"
.LASF439:
	.ascii	"UINT_FAST8_MAX UINT8_MAX\000"
.LASF320:
	.ascii	"__SA_IBIT__ 16\000"
.LASF250:
	.ascii	"__LLFRACT_MAX__ 0X7FFFFFFFFFFFFFFFP-63LLR\000"
.LASF141:
	.ascii	"__INT_FAST64_MAX__ 0x7fffffffffffffffLL\000"
.LASF167:
	.ascii	"__FP_FAST_FMAF 1\000"
.LASF639:
	.ascii	"tsGetXY\000"
.LASF139:
	.ascii	"__INT_FAST16_MAX__ 0x7fffffff\000"
.LASF342:
	.ascii	"__GCC_ATOMIC_BOOL_LOCK_FREE 2\000"
.LASF340:
	.ascii	"__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2 1\000"
.LASF412:
	.ascii	"INT32_MIN (-2147483647L-1)\000"
.LASF321:
	.ascii	"__DA_FBIT__ 31\000"
.LASF367:
	.ascii	"__ARM_SIZEOF_MINIMAL_ENUM 1\000"
.LASF521:
	.ascii	"FT5336_P3_YL_REG ((uint8_t)0x12)\000"
.LASF663:
	.ascii	"TS_Configure\000"
.LASF522:
	.ascii	"FT5336_P3_WEIGHT_REG ((uint8_t)0x13)\000"
.LASF148:
	.ascii	"__GCC_IEC_559 0\000"
.LASF605:
	.ascii	"long long unsigned int\000"
.LASF266:
	.ascii	"__USACCUM_EPSILON__ 0x1P-8UHK\000"
.LASF568:
	.ascii	"FT5336_THRESHOLD_SHIFT ((uint8_t)0x00)\000"
.LASF106:
	.ascii	"__PTRDIFF_MAX__ 0x7fffffff\000"
.LASF462:
	.ascii	"WINT_MAX 2147483647L\000"
.LASF285:
	.ascii	"__ULACCUM_MAX__ 0XFFFFFFFFFFFFFFFFP-32ULK\000"
.LASF235:
	.ascii	"__UFRACT_MAX__ 0XFFFFP-16UR\000"
.LASF220:
	.ascii	"__SFRACT_MAX__ 0X7FP-7HR\000"
.LASF387:
	.ascii	"__ARM_EABI__ 1\000"
.LASF529:
	.ascii	"FT5336_P4_MISC_REG ((uint8_t)0x1A)\000"
.LASF130:
	.ascii	"__UINT_LEAST8_MAX__ 0xff\000"
.LASF211:
	.ascii	"__DEC128_MIN_EXP__ (-6142)\000"
.LASF570:
	.ascii	"FT5336_CTRL_REG ((uint8_t)0x86)\000"
.LASF114:
	.ascii	"__INT8_MAX__ 0x7f\000"
.LASF246:
	.ascii	"__ULFRACT_EPSILON__ 0x1P-32ULR\000"
.LASF215:
	.ascii	"__DEC128_EPSILON__ 1E-33DL\000"
.LASF546:
	.ascii	"FT5336_P7_WEIGHT_REG ((uint8_t)0x2B)\000"
.LASF243:
	.ascii	"__ULFRACT_IBIT__ 0\000"
.LASF287:
	.ascii	"__LLACCUM_FBIT__ 31\000"
.LASF477:
	.ascii	"FT5336_GEST_ID_REG ((uint8_t)0x01)\000"
.LASF255:
	.ascii	"__ULLFRACT_MAX__ 0XFFFFFFFFFFFFFFFFP-64ULLR\000"
.LASF136:
	.ascii	"__UINT_LEAST64_MAX__ 0xffffffffffffffffULL\000"
.LASF152:
	.ascii	"__FLT_RADIX__ 2\000"
.LASF505:
	.ascii	"FT5336_P1_YL_REG ((uint8_t)0x06)\000"
.LASF175:
	.ascii	"__DBL_MAX__ double(1.1)\000"
.LASF254:
	.ascii	"__ULLFRACT_MIN__ 0.0ULLR\000"
.LASF181:
	.ascii	"__DBL_HAS_QUIET_NAN__ 1\000"
.LASF274:
	.ascii	"__UACCUM_MIN__ 0.0UK\000"
.LASF241:
	.ascii	"__LFRACT_EPSILON__ 0x1P-31LR\000"
.LASF59:
	.ascii	"__INT_FAST8_TYPE__ int\000"
.LASF532:
	.ascii	"FT5336_P5_YH_REG ((uint8_t)0x1D)\000"
.LASF471:
	.ascii	"FT5336_MAX_DETECTABLE_TOUCH ((uint8_t)0x05)\000"
.LASF64:
	.ascii	"__UINT_FAST16_TYPE__ unsigned int\000"
.LASF578:
	.ascii	"FT5336_OFFSET_UP_DOWN_REG ((uint8_t)0x93)\000"
.LASF563:
	.ascii	"FT5336_P10_YL_REG ((uint8_t)0x3C)\000"
.LASF478:
	.ascii	"FT5336_GEST_ID_NO_GESTURE ((uint8_t)0x00)\000"
.LASF257:
	.ascii	"__SACCUM_FBIT__ 7\000"
.LASF115:
	.ascii	"__INT16_MAX__ 0x7fff\000"
.LASF369:
	.ascii	"__ARM_ARCH_PROFILE 77\000"
.LASF68:
	.ascii	"__UINTPTR_TYPE__ unsigned int\000"
.LASF658:
	.ascii	"TS_IO_Delay\000"
.LASF229:
	.ascii	"__FRACT_MIN__ (-0.5R-0.5R)\000"
.LASF88:
	.ascii	"__cpp_rvalue_references 200610\000"
.LASF108:
	.ascii	"__INTMAX_MAX__ 0x7fffffffffffffffLL\000"
.LASF276:
	.ascii	"__UACCUM_EPSILON__ 0x1P-16UK\000"
.LASF616:
	.ascii	"TS_DrvTypeDef\000"
.LASF390:
	.ascii	"__ARM_ASM_SYNTAX_UNIFIED__ 1\000"
.LASF268:
	.ascii	"__ACCUM_IBIT__ 16\000"
.LASF525:
	.ascii	"FT5336_P4_XL_REG ((uint8_t)0x16)\000"
.LASF304:
	.ascii	"__DQ_IBIT__ 0\000"
.LASF262:
	.ascii	"__USACCUM_FBIT__ 8\000"
.LASF258:
	.ascii	"__SACCUM_IBIT__ 8\000"
.LASF356:
	.ascii	"__SIZEOF_PTRDIFF_T__ 4\000"
.LASF408:
	.ascii	"INT16_MIN (-32767-1)\000"
.LASF219:
	.ascii	"__SFRACT_MIN__ (-0.5HR-0.5HR)\000"
.LASF328:
	.ascii	"__USA_IBIT__ 16\000"
.LASF459:
	.ascii	"WCHAR_MIN (-2147483647L-1)\000"
.LASF210:
	.ascii	"__DEC128_MANT_DIG__ 34\000"
.LASF388:
	.ascii	"__ARM_ARCH_EXT_IDIV__ 1\000"
.LASF612:
	.ascii	"EnableIT\000"
.LASF447:
	.ascii	"INTPTR_MAX INT32_MAX\000"
.LASF516:
	.ascii	"FT5336_P2_WEIGHT_REG ((uint8_t)0x0D)\000"
.LASF21:
	.ascii	"__SIZEOF_FLOAT__ 4\000"
.LASF593:
	.ascii	"FT5336_ID_VALUE ((uint8_t)0x51)\000"
.LASF135:
	.ascii	"__UINT32_C(c) c ## UL\000"
.LASF198:
	.ascii	"__DEC32_MAX_EXP__ 97\000"
.LASF606:
	.ascii	"Init\000"
.LASF411:
	.ascii	"INT32_MAX 2147483647L\000"
.LASF534:
	.ascii	"FT5336_P5_WEIGHT_REG ((uint8_t)0x1F)\000"
.LASF551:
	.ascii	"FT5336_P8_YL_REG ((uint8_t)0x30)\000"
.LASF94:
	.ascii	"__cpp_ref_qualifiers 200710\000"
.LASF5:
	.ascii	"__GNUC__ 6\000"
.LASF544:
	.ascii	"FT5336_P7_YH_REG ((uint8_t)0x29)\000"
.LASF618:
	.ascii	"21ft5336_handle_TypeDef\000"
.LASF338:
	.ascii	"__WCHAR_UNSIGNED__ 1\000"
.LASF71:
	.ascii	"__GXX_WEAK__ 1\000"
.LASF375:
	.ascii	"__THUMBEL__ 1\000"
.LASF190:
	.ascii	"__LDBL_MIN__ 1.1\000"
.LASF322:
	.ascii	"__DA_IBIT__ 32\000"
.LASF26:
	.ascii	"__BIGGEST_ALIGNMENT__ 8\000"
.LASF278:
	.ascii	"__LACCUM_IBIT__ 32\000"
.LASF225:
	.ascii	"__USFRACT_MAX__ 0XFFP-8UHR\000"
.LASF142:
	.ascii	"__UINT_FAST8_MAX__ 0xffffffffU\000"
.LASF653:
	.ascii	"bFoundDevice\000"
.LASF645:
	.ascii	"tsDetectTouch\000"
.LASF289:
	.ascii	"__LLACCUM_MIN__ (-0X1P31LLK-0X1P31LLK)\000"
.LASF436:
	.ascii	"INT_FAST16_MAX INT32_MAX\000"
.LASF625:
	.ascii	"gTsHandle\000"
.LASF319:
	.ascii	"__SA_FBIT__ 15\000"
.LASF398:
	.ascii	"USE_HAL_DRIVER 1\000"
.LASF611:
	.ascii	"GetXY\000"
.LASF92:
	.ascii	"__cpp_nsdmi 200809\000"
.LASF261:
	.ascii	"__SACCUM_EPSILON__ 0x1P-7HK\000"
.LASF631:
	.ascii	"pArea\000"
.LASF60:
	.ascii	"__INT_FAST16_TYPE__ int\000"
.LASF419:
	.ascii	"INT_LEAST8_MIN INT8_MIN\000"
.LASF48:
	.ascii	"__UINT16_TYPE__ short unsigned int\000"
.LASF512:
	.ascii	"FT5336_P2_XH_REG ((uint8_t)0x09)\000"
.LASF335:
	.ascii	"__GNUC_STDC_INLINE__ 1\000"
.LASF310:
	.ascii	"__UHQ_IBIT__ 0\000"
.LASF232:
	.ascii	"__UFRACT_FBIT__ 16\000"
.LASF442:
	.ascii	"UINT_FAST64_MAX UINT64_MAX\000"
.LASF365:
	.ascii	"__ARM_FEATURE_CLZ 1\000"
.LASF101:
	.ascii	"__LONG_LONG_MAX__ 0x7fffffffffffffffLL\000"
.LASF424:
	.ascii	"INT_LEAST16_MAX INT16_MAX\000"
.LASF46:
	.ascii	"__INT64_TYPE__ long long int\000"
.LASF158:
	.ascii	"__FLT_MAX_10_EXP__ 38\000"
.LASF556:
	.ascii	"FT5336_P9_YH_REG ((uint8_t)0x35)\000"
.LASF634:
	.ascii	"regAddressPWeight\000"
.LASF494:
	.ascii	"FT5336_TOUCH_EVT_FLAG_CONTACT ((uint8_t)0x02)\000"
.LASF27:
	.ascii	"__ORDER_LITTLE_ENDIAN__ 1234\000"
.LASF291:
	.ascii	"__LLACCUM_EPSILON__ 0x1P-31LLK\000"
.LASF81:
	.ascii	"__cpp_lambdas 200907\000"
.LASF607:
	.ascii	"ReadID\000"
.LASF244:
	.ascii	"__ULFRACT_MIN__ 0.0ULR\000"
.LASF153:
	.ascii	"__FLT_MANT_DIG__ 24\000"
.LASF312:
	.ascii	"__USQ_IBIT__ 0\000"
.LASF585:
	.ascii	"FT5336_GMODE_REG ((uint8_t)0xA4)\000"
.LASF635:
	.ascii	"regAddressPMisc\000"
.LASF76:
	.ascii	"__cpp_runtime_arrays 198712\000"
.LASF481:
	.ascii	"FT5336_GEST_ID_MOVE_DOWN ((uint8_t)0x18)\000"
.LASF660:
	.ascii	"GNU C++11 6.3.1 20170620 (release) [ARM/embedded-6-"
	.ascii	"branch revision 249437] -fmessage-length=0 -fno-exc"
	.ascii	"eptions -fno-rtti -mcpu=cortex-m7 -mlittle-endian -"
	.ascii	"mfloat-abi=hard -mfpu=fpv5-sp-d16 -mthumb -mtp=soft"
	.ascii	" -g3 -gpubnames -std=c++11 -Og -fomit-frame-pointer"
	.ascii	" -fno-dwarf2-cfi-asm -fno-builtin -ffunction-sectio"
	.ascii	"ns -fdata-sections -fshort-enums -fno-common\000"
.LASF273:
	.ascii	"__UACCUM_IBIT__ 16\000"
.LASF143:
	.ascii	"__UINT_FAST16_MAX__ 0xffffffffU\000"
.LASF533:
	.ascii	"FT5336_P5_YL_REG ((uint8_t)0x1E)\000"
.LASF80:
	.ascii	"__cpp_user_defined_literals 200809\000"
.LASF171:
	.ascii	"__DBL_MIN_10_EXP__ (-307)\000"
.LASF263:
	.ascii	"__USACCUM_IBIT__ 8\000"
.LASF467:
	.ascii	"FT5336_STATUS_OK ((uint8_t)0x00)\000"
.LASF384:
	.ascii	"__THUMB_INTERWORK__ 1\000"
.LASF559:
	.ascii	"FT5336_P9_MISC_REG ((uint8_t)0x38)\000"
.LASF227:
	.ascii	"__FRACT_FBIT__ 15\000"
.LASF430:
	.ascii	"UINT_LEAST64_MAX UINT64_MAX\000"
.LASF403:
	.ascii	"__stdint_H \000"
.LASF150:
	.ascii	"__FLT_EVAL_METHOD__ 0\000"
.LASF629:
	.ascii	"touchIdx\000"
.LASF93:
	.ascii	"__cpp_inheriting_constructors 200802\000"
.LASF426:
	.ascii	"INT_LEAST64_MAX INT64_MAX\000"
.LASF128:
	.ascii	"__INT_LEAST64_MAX__ 0x7fffffffffffffffLL\000"
.LASF267:
	.ascii	"__ACCUM_FBIT__ 15\000"
.LASF432:
	.ascii	"INT_FAST16_MIN INT32_MIN\000"
.LASF204:
	.ascii	"__DEC64_MIN_EXP__ (-382)\000"
.LASF283:
	.ascii	"__ULACCUM_IBIT__ 32\000"
.LASF247:
	.ascii	"__LLFRACT_FBIT__ 63\000"
.LASF549:
	.ascii	"FT5336_P8_XL_REG ((uint8_t)0x2E)\000"
.LASF470:
	.ascii	"FT5336_I2C_INITIALIZED ((uint8_t)0x01)\000"
.LASF327:
	.ascii	"__USA_FBIT__ 16\000"
.LASF118:
	.ascii	"__UINT8_MAX__ 0xff\000"
.LASF145:
	.ascii	"__UINT_FAST64_MAX__ 0xffffffffffffffffULL\000"
.LASF119:
	.ascii	"__UINT16_MAX__ 0xffff\000"
.LASF397:
	.ascii	"DEBUG 1\000"
.LASF161:
	.ascii	"__FLT_MIN__ 1.1\000"
.LASF404:
	.ascii	"UINT8_MAX 255\000"
.LASF599:
	.ascii	"uint8_t\000"
.LASF513:
	.ascii	"FT5336_P2_XL_REG ((uint8_t)0x0A)\000"
.LASF444:
	.ascii	"PTRDIFF_MAX INT32_MAX\000"
.LASF200:
	.ascii	"__DEC32_MAX__ 9.999999E96DF\000"
.LASF237:
	.ascii	"__LFRACT_FBIT__ 31\000"
.LASF461:
	.ascii	"WINT_MIN (-2147483647L-1)\000"
.LASF52:
	.ascii	"__INT_LEAST16_TYPE__ short int\000"
.LASF573:
	.ascii	"FT5336_TIMEENTERMONITOR_REG ((uint8_t)0x87)\000"
.LASF169:
	.ascii	"__DBL_DIG__ 15\000"
.LASF344:
	.ascii	"__GCC_ATOMIC_CHAR16_T_LOCK_FREE 2\000"
.LASF122:
	.ascii	"__INT_LEAST8_MAX__ 0x7f\000"
.LASF628:
	.ascii	"ft5336_TS_GetTouchInfo\000"
.LASF316:
	.ascii	"__UTQ_IBIT__ 0\000"
.LASF236:
	.ascii	"__UFRACT_EPSILON__ 0x1P-16UR\000"
.LASF545:
	.ascii	"FT5336_P7_YL_REG ((uint8_t)0x2A)\000"
.LASF349:
	.ascii	"__GCC_ATOMIC_LONG_LOCK_FREE 2\000"
.LASF271:
	.ascii	"__ACCUM_EPSILON__ 0x1P-15K\000"
.LASF418:
	.ascii	"UINTMAX_MAX 18446744073709551615ULL\000"
.LASF313:
	.ascii	"__UDQ_FBIT__ 64\000"
.LASF498:
	.ascii	"FT5336_TOUCH_POS_MSB_MASK ((uint8_t)0x0F)\000"
.LASF595:
	.ascii	"FT5336_STATE_REG ((uint8_t)0xBC)\000"
.LASF72:
	.ascii	"__DEPRECATED 1\000"
.LASF376:
	.ascii	"__ARM_ARCH_ISA_THUMB 2\000"
.LASF315:
	.ascii	"__UTQ_FBIT__ 128\000"
.LASF604:
	.ascii	"long long int\000"
.LASF25:
	.ascii	"__CHAR_BIT__ 8\000"
.LASF552:
	.ascii	"FT5336_P8_WEIGHT_REG ((uint8_t)0x31)\000"
.LASF501:
	.ascii	"FT5336_TOUCH_POS_LSB_SHIFT ((uint8_t)0x00)\000"
.LASF454:
	.ascii	"UINT32_C(x) (x ##UL)\000"
.LASF55:
	.ascii	"__UINT_LEAST8_TYPE__ unsigned char\000"
.LASF16:
	.ascii	"__FINITE_MATH_ONLY__ 0\000"
.LASF260:
	.ascii	"__SACCUM_MAX__ 0X7FFFP-7HK\000"
.LASF370:
	.ascii	"__arm__ 1\000"
.LASF90:
	.ascii	"__cpp_initializer_lists 200806\000"
.LASF465:
	.ascii	"FT5336_MAX_WIDTH ((uint16_t)480)\000"
.LASF182:
	.ascii	"__LDBL_MANT_DIG__ 53\000"
.LASF306:
	.ascii	"__TQ_IBIT__ 0\000"
.LASF325:
	.ascii	"__UHA_FBIT__ 8\000"
.LASF420:
	.ascii	"INT_LEAST16_MIN INT16_MIN\000"
.LASF567:
	.ascii	"FT5336_THRESHOLD_MASK ((uint8_t)0xFF)\000"
.LASF548:
	.ascii	"FT5336_P8_XH_REG ((uint8_t)0x2D)\000"
.LASF393:
	.ascii	"__ELF__ 1\000"
.LASF455:
	.ascii	"INT64_C(x) (x ##LL)\000"
.LASF361:
	.ascii	"__ARM_FEATURE_UNALIGNED 1\000"
.LASF125:
	.ascii	"__INT16_C(c) c\000"
.LASF124:
	.ascii	"__INT_LEAST16_MAX__ 0x7fff\000"
.LASF121:
	.ascii	"__UINT64_MAX__ 0xffffffffffffffffULL\000"
.LASF280:
	.ascii	"__LACCUM_MAX__ 0X7FFFFFFFFFFFFFFFP-31LK\000"
.LASF140:
	.ascii	"__INT_FAST32_MAX__ 0x7fffffff\000"
.LASF226:
	.ascii	"__USFRACT_EPSILON__ 0x1P-8UHR\000"
.LASF11:
	.ascii	"__ATOMIC_ACQUIRE 2\000"
.LASF464:
	.ascii	"FT5336_I2C_SLAVE_ADDRESS ((uint8_t)0x70)\000"
.LASF626:
	.ascii	"ucReadData\000"
.LASF117:
	.ascii	"__INT64_MAX__ 0x7fffffffffffffffLL\000"
.LASF491:
	.ascii	"FT5336_TD_STAT_SHIFT ((uint8_t)0x00)\000"
.LASF391:
	.ascii	"__ARM_FEATURE_COPROC 15\000"
.LASF0:
	.ascii	"__STDC__ 1\000"
.LASF272:
	.ascii	"__UACCUM_FBIT__ 16\000"
.LASF172:
	.ascii	"__DBL_MAX_EXP__ 1024\000"
.LASF503:
	.ascii	"FT5336_P1_XL_REG ((uint8_t)0x04)\000"
.LASF14:
	.ascii	"__ATOMIC_CONSUME 1\000"
.LASF504:
	.ascii	"FT5336_P1_YH_REG ((uint8_t)0x05)\000"
.LASF537:
	.ascii	"FT5336_P6_XL_REG ((uint8_t)0x22)\000"
.LASF77:
	.ascii	"__cpp_unicode_characters 200704\000"
.LASF37:
	.ascii	"__WINT_TYPE__ unsigned int\000"
.LASF510:
	.ascii	"FT5336_TOUCH_AREA_MASK ((uint8_t)(0x04 << 4))\000"
.LASF100:
	.ascii	"__LONG_MAX__ 0x7fffffffL\000"
.LASF82:
	.ascii	"__cpp_constexpr 200704\000"
.LASF242:
	.ascii	"__ULFRACT_FBIT__ 32\000"
.LASF36:
	.ascii	"__WCHAR_TYPE__ unsigned int\000"
.LASF539:
	.ascii	"FT5336_P6_YL_REG ((uint8_t)0x24)\000"
.LASF269:
	.ascii	"__ACCUM_MIN__ (-0X1P15K-0X1P15K)\000"
.LASF615:
	.ascii	"DisableIT\000"
.LASF22:
	.ascii	"__SIZEOF_DOUBLE__ 8\000"
.LASF507:
	.ascii	"FT5336_TOUCH_WEIGHT_MASK ((uint8_t)0xFF)\000"
.LASF7:
	.ascii	"__GNUC_PATCHLEVEL__ 1\000"
.LASF73:
	.ascii	"__GXX_EXPERIMENTAL_CXX0X__ 1\000"
.LASF281:
	.ascii	"__LACCUM_EPSILON__ 0x1P-31LK\000"
.LASF230:
	.ascii	"__FRACT_MAX__ 0X7FFFP-15R\000"
.LASF105:
	.ascii	"__WINT_MIN__ 0U\000"
.LASF282:
	.ascii	"__ULACCUM_FBIT__ 32\000"
.LASF234:
	.ascii	"__UFRACT_MIN__ 0.0UR\000"
.LASF383:
	.ascii	"__ARM_NEON_FP\000"
.LASF15:
	.ascii	"__OPTIMIZE__ 1\000"
.LASF2:
	.ascii	"__STDC_UTF_16__ 1\000"
.LASF228:
	.ascii	"__FRACT_IBIT__ 0\000"
.LASF535:
	.ascii	"FT5336_P5_MISC_REG ((uint8_t)0x20)\000"
.LASF323:
	.ascii	"__TA_FBIT__ 63\000"
.LASF194:
	.ascii	"__LDBL_HAS_INFINITY__ 1\000"
.LASF149:
	.ascii	"__GCC_IEC_559_COMPLEX 0\000"
.LASF120:
	.ascii	"__UINT32_MAX__ 0xffffffffUL\000"
.LASF648:
	.ascii	"tsDisableIT\000"
.LASF184:
	.ascii	"__LDBL_MIN_EXP__ (-1021)\000"
.LASF252:
	.ascii	"__ULLFRACT_FBIT__ 64\000"
.LASF354:
	.ascii	"__SIZEOF_WCHAR_T__ 4\000"
.LASF483:
	.ascii	"FT5336_GEST_ID_SINGLE_CLICK ((uint8_t)0x20)\000"
.LASF31:
	.ascii	"__FLOAT_WORD_ORDER__ __ORDER_LITTLE_ENDIAN__\000"
.LASF270:
	.ascii	"__ACCUM_MAX__ 0X7FFFFFFFP-15K\000"
.LASF233:
	.ascii	"__UFRACT_IBIT__ 0\000"
.LASF196:
	.ascii	"__DEC32_MANT_DIG__ 7\000"
.LASF575:
	.ascii	"FT5336_PERIODMONITOR_REG ((uint8_t)0x89)\000"
.LASF177:
	.ascii	"__DBL_EPSILON__ double(1.1)\000"
.LASF569:
	.ascii	"FT5336_TH_DIFF_REG ((uint8_t)0x85)\000"
.LASF113:
	.ascii	"__SIG_ATOMIC_MIN__ (-__SIG_ATOMIC_MAX__ - 1)\000"
.LASF251:
	.ascii	"__LLFRACT_EPSILON__ 0x1P-63LLR\000"
.LASF399:
	.ascii	"STM32F746xx 1\000"
.LASF456:
	.ascii	"UINT64_C(x) (x ##ULL)\000"
.LASF540:
	.ascii	"FT5336_P6_WEIGHT_REG ((uint8_t)0x25)\000"
.LASF187:
	.ascii	"__LDBL_MAX_10_EXP__ 308\000"
.LASF217:
	.ascii	"__SFRACT_FBIT__ 7\000"
.LASF41:
	.ascii	"__CHAR32_TYPE__ long unsigned int\000"
.LASF520:
	.ascii	"FT5336_P3_YH_REG ((uint8_t)0x11)\000"
.LASF558:
	.ascii	"FT5336_P9_WEIGHT_REG ((uint8_t)0x37)\000"
.LASF415:
	.ascii	"UINT64_MAX 18446744073709551615ULL\000"
.LASF79:
	.ascii	"__cpp_unicode_literals 200710\000"
.LASF300:
	.ascii	"__HQ_IBIT__ 0\000"
.LASF496:
	.ascii	"FT5336_TOUCH_EVT_FLAG_SHIFT ((uint8_t)0x06)\000"
.LASF600:
	.ascii	"uint16_t\000"
.LASF334:
	.ascii	"__USER_LABEL_PREFIX__ \000"
.LASF526:
	.ascii	"FT5336_P4_YH_REG ((uint8_t)0x17)\000"
.LASF493:
	.ascii	"FT5336_TOUCH_EVT_FLAG_LIFT_UP ((uint8_t)0x01)\000"
.LASF57:
	.ascii	"__UINT_LEAST32_TYPE__ long unsigned int\000"
.LASF659:
	.ascii	"TS_IO_Init\000"
.LASF303:
	.ascii	"__DQ_FBIT__ 63\000"
.LASF265:
	.ascii	"__USACCUM_MAX__ 0XFFFFP-8UHK\000"
.LASF565:
	.ascii	"FT5336_P10_MISC_REG ((uint8_t)0x3E)\000"
.LASF364:
	.ascii	"__ARM_FEATURE_LDREX 7\000"
.LASF353:
	.ascii	"__PRAGMA_REDEFINE_EXTNAME 1\000"
.LASF410:
	.ascii	"UINT32_MAX 4294967295UL\000"
.LASF592:
	.ascii	"FT5336_CHIP_ID_REG ((uint8_t)0xA8)\000"
.LASF98:
	.ascii	"__SHRT_MAX__ 0x7fff\000"
.LASF346:
	.ascii	"__GCC_ATOMIC_WCHAR_T_LOCK_FREE 2\000"
.LASF594:
	.ascii	"FT5336_RELEASE_CODE_ID_REG ((uint8_t)0xAF)\000"
.LASF29:
	.ascii	"__ORDER_PDP_ENDIAN__ 3412\000"
.LASF583:
	.ascii	"FT5336_LIB_VER_L_REG ((uint8_t)0xA2)\000"
.LASF302:
	.ascii	"__SQ_IBIT__ 0\000"
.LASF488:
	.ascii	"FT5336_GEST_ID_ZOOM_OUT ((uint8_t)0x49)\000"
.LASF422:
	.ascii	"INT_LEAST64_MIN INT64_MIN\000"
.LASF159:
	.ascii	"__FLT_DECIMAL_DIG__ 9\000"
.LASF584:
	.ascii	"FT5336_CIPHER_REG ((uint8_t)0xA3)\000"
.LASF116:
	.ascii	"__INT32_MAX__ 0x7fffffffL\000"
.LASF492:
	.ascii	"FT5336_TOUCH_EVT_FLAG_PRESS_DOWN ((uint8_t)0x00)\000"
.LASF406:
	.ascii	"INT8_MIN (-128)\000"
.LASF96:
	.ascii	"__GXX_ABI_VERSION 1010\000"
.LASF449:
	.ascii	"INT8_C(x) (x)\000"
.LASF469:
	.ascii	"FT5336_I2C_NOT_INITIALIZED ((uint8_t)0x00)\000"
.LASF307:
	.ascii	"__UQQ_FBIT__ 8\000"
.LASF579:
	.ascii	"FT5336_DISTANCE_LEFT_RIGHT_REG ((uint8_t)0x94)\000"
.LASF17:
	.ascii	"__SIZEOF_INT__ 4\000"
.LASF598:
	.ascii	"short int\000"
.LASF275:
	.ascii	"__UACCUM_MAX__ 0XFFFFFFFFP-16UK\000"
.LASF614:
	.ascii	"GetITStatus\000"
.LASF591:
	.ascii	"FT5336_FIRMID_REG ((uint8_t)0xA6)\000"
.LASF205:
	.ascii	"__DEC64_MAX_EXP__ 385\000"
.LASF580:
	.ascii	"FT5336_DISTANCE_UP_DOWN_REG ((uint8_t)0x95)\000"
.LASF425:
	.ascii	"INT_LEAST32_MAX INT32_MAX\000"
.LASF45:
	.ascii	"__INT32_TYPE__ long int\000"
.LASF75:
	.ascii	"__cpp_hex_float 201603\000"
.LASF389:
	.ascii	"__ARM_FEATURE_IDIV 1\000"
.LASF531:
	.ascii	"FT5336_P5_XL_REG ((uint8_t)0x1C)\000"
.LASF186:
	.ascii	"__LDBL_MAX_EXP__ 1024\000"
.LASF301:
	.ascii	"__SQ_FBIT__ 31\000"
.LASF509:
	.ascii	"FT5336_P1_MISC_REG ((uint8_t)0x08)\000"
.LASF239:
	.ascii	"__LFRACT_MIN__ (-0.5LR-0.5LR)\000"
.LASF446:
	.ascii	"INTPTR_MIN INT32_MIN\000"
.LASF212:
	.ascii	"__DEC128_MAX_EXP__ 6145\000"
.LASF33:
	.ascii	"__GNUG__ 6\000"
.LASF405:
	.ascii	"INT8_MAX 127\000"
.LASF134:
	.ascii	"__UINT_LEAST32_MAX__ 0xffffffffUL\000"
.LASF542:
	.ascii	"FT5336_P7_XH_REG ((uint8_t)0x27)\000"
.LASF431:
	.ascii	"INT_FAST8_MIN INT8_MIN\000"
.LASF373:
	.ascii	"__thumb__ 1\000"
.LASF277:
	.ascii	"__LACCUM_FBIT__ 31\000"
.LASF195:
	.ascii	"__LDBL_HAS_QUIET_NAN__ 1\000"
.LASF43:
	.ascii	"__INT8_TYPE__ signed char\000"
.LASF104:
	.ascii	"__WINT_MAX__ 0xffffffffU\000"
.LASF644:
	.ascii	"tsITStatus\000"
.LASF623:
	.ascii	"deviceAddr\000"
.LASF636:
	.ascii	"tsClearIT\000"
.LASF656:
	.ascii	"TS_IO_Read\000"
.LASF176:
	.ascii	"__DBL_MIN__ double(1.1)\000"
.LASF622:
	.ascii	"ft5336_handle_TypeDef\000"
.LASF305:
	.ascii	"__TQ_FBIT__ 127\000"
.LASF309:
	.ascii	"__UHQ_FBIT__ 16\000"
.LASF199:
	.ascii	"__DEC32_MIN__ 1E-95DF\000"
.LASF414:
	.ascii	"INT64_MAX 9223372036854775807LL\000"
.LASF293:
	.ascii	"__ULLACCUM_IBIT__ 32\000"
.LASF144:
	.ascii	"__UINT_FAST32_MAX__ 0xffffffffU\000"
.LASF222:
	.ascii	"__USFRACT_FBIT__ 8\000"
.LASF129:
	.ascii	"__INT64_C(c) c ## LL\000"
.LASF299:
	.ascii	"__HQ_FBIT__ 15\000"
.LASF326:
	.ascii	"__UHA_IBIT__ 8\000"
.LASF642:
	.ascii	"regAddressYLow\000"
.LASF366:
	.ascii	"__ARM_FEATURE_SIMD32 1\000"
.LASF218:
	.ascii	"__SFRACT_IBIT__ 0\000"
.LASF649:
	.ascii	"reset\000"
.LASF19:
	.ascii	"__SIZEOF_LONG_LONG__ 8\000"
.LASF308:
	.ascii	"__UQQ_IBIT__ 0\000"
.LASF47:
	.ascii	"__UINT8_TYPE__ unsigned char\000"
.LASF536:
	.ascii	"FT5336_P6_XH_REG ((uint8_t)0x21)\000"
.LASF500:
	.ascii	"FT5336_TOUCH_POS_LSB_MASK ((uint8_t)0xFF)\000"
.LASF336:
	.ascii	"__STRICT_ANSI__ 1\000"
.LASF562:
	.ascii	"FT5336_P10_YH_REG ((uint8_t)0x3B)\000"
.LASF560:
	.ascii	"FT5336_P10_XH_REG ((uint8_t)0x39)\000"
.LASF413:
	.ascii	"INT64_MIN (-9223372036854775807LL-1)\000"
.LASF54:
	.ascii	"__INT_LEAST64_TYPE__ long long int\000"
.LASF555:
	.ascii	"FT5336_P9_XL_REG ((uint8_t)0x34)\000"
.LASF49:
	.ascii	"__UINT32_TYPE__ long unsigned int\000"
.LASF185:
	.ascii	"__LDBL_MIN_10_EXP__ (-307)\000"
.LASF32:
	.ascii	"__SIZEOF_POINTER__ 4\000"
.LASF643:
	.ascii	"regAddressYHigh\000"
.LASF487:
	.ascii	"FT5336_GEST_ID_ZOOM_IN ((uint8_t)0x40)\000"
.LASF207:
	.ascii	"__DEC64_MAX__ 9.999999999999999E384DD\000"
.LASF245:
	.ascii	"__ULFRACT_MAX__ 0XFFFFFFFFP-32ULR\000"
.LASF489:
	.ascii	"FT5336_TD_STAT_REG ((uint8_t)0x02)\000"
.LASF317:
	.ascii	"__HA_FBIT__ 7\000"
.LASF421:
	.ascii	"INT_LEAST32_MIN INT32_MIN\000"
.LASF524:
	.ascii	"FT5336_P4_XH_REG ((uint8_t)0x15)\000"
.LASF259:
	.ascii	"__SACCUM_MIN__ (-0X1P7HK-0X1P7HK)\000"
.LASF554:
	.ascii	"FT5336_P9_XH_REG ((uint8_t)0x33)\000"
.LASF352:
	.ascii	"__GCC_ATOMIC_POINTER_LOCK_FREE 2\000"
.LASF137:
	.ascii	"__UINT64_C(c) c ## ULL\000"
.LASF561:
	.ascii	"FT5336_P10_XL_REG ((uint8_t)0x3A)\000"
.LASF654:
	.ascii	"init\000"
.LASF448:
	.ascii	"UINTPTR_MAX UINT32_MAX\000"
.LASF221:
	.ascii	"__SFRACT_EPSILON__ 0x1P-7HR\000"
.LASF74:
	.ascii	"__cpp_binary_literals 201304\000"
.LASF661:
	.ascii	"C:\\projects\\segger\\common\\ft5336.cpp\000"
.LASF318:
	.ascii	"__HA_IBIT__ 8\000"
.LASF482:
	.ascii	"FT5336_GEST_ID_MOVE_LEFT ((uint8_t)0x1C)\000"
.LASF514:
	.ascii	"FT5336_P2_YH_REG ((uint8_t)0x0B)\000"
.LASF345:
	.ascii	"__GCC_ATOMIC_CHAR32_T_LOCK_FREE 2\000"
.LASF486:
	.ascii	"FT5336_GEST_ID_ROTATE_C_CLOCKWISE ((uint8_t)0x29)\000"
.LASF571:
	.ascii	"FT5336_CTRL_KEEP_ACTIVE_MODE ((uint8_t)0x00)\000"
.LASF517:
	.ascii	"FT5336_P2_MISC_REG ((uint8_t)0x0E)\000"
.LASF163:
	.ascii	"__FLT_DENORM_MIN__ 1.1\000"
.LASF650:
	.ascii	"readId\000"
.LASF630:
	.ascii	"pWeight\000"
.LASF214:
	.ascii	"__DEC128_MAX__ 9.999999999999999999999999999999999E"
	.ascii	"6144DL\000"
.LASF231:
	.ascii	"__FRACT_EPSILON__ 0x1P-15R\000"
.LASF4:
	.ascii	"__STDC_HOSTED__ 1\000"
.LASF146:
	.ascii	"__INTPTR_MAX__ 0x7fffffff\000"
.LASF69:
	.ascii	"__has_include(STR) __has_include__(STR)\000"
.LASF632:
	.ascii	"pEvent\000"
.LASF394:
	.ascii	"__SIZEOF_WCHAR_T 4\000"
.LASF434:
	.ascii	"INT_FAST64_MIN INT64_MIN\000"
.LASF473:
	.ascii	"FT5336_DEV_MODE_WORKING ((uint8_t)0x00)\000"
.LASF409:
	.ascii	"INT16_MAX 32767\000"
.LASF452:
	.ascii	"UINT16_C(x) (x ##U)\000"
.LASF350:
	.ascii	"__GCC_ATOMIC_LLONG_LOCK_FREE 1\000"
.LASF341:
	.ascii	"__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4 1\000"
.LASF157:
	.ascii	"__FLT_MAX_EXP__ 128\000"
.LASF13:
	.ascii	"__ATOMIC_ACQ_REL 4\000"
.LASF392:
	.ascii	"__GXX_TYPEINFO_EQUALITY_INLINE 0\000"
.LASF597:
	.ascii	"unsigned char\000"
.LASF197:
	.ascii	"__DEC32_MIN_EXP__ (-94)\000"
.LASF39:
	.ascii	"__UINTMAX_TYPE__ long long unsigned int\000"
.LASF564:
	.ascii	"FT5336_P10_WEIGHT_REG ((uint8_t)0x3D)\000"
.LASF502:
	.ascii	"FT5336_P1_XH_REG ((uint8_t)0x03)\000"
.LASF655:
	.ascii	"I2C_InitializeIfRequired\000"
.LASF67:
	.ascii	"__INTPTR_TYPE__ int\000"
.LASF386:
	.ascii	"__ARM_PCS_VFP 1\000"
.LASF151:
	.ascii	"__DEC_EVAL_METHOD__ 2\000"
.LASF264:
	.ascii	"__USACCUM_MIN__ 0.0UHK\000"
.LASF87:
	.ascii	"__cpp_rvalue_reference 200610\000"
.LASF662:
	.ascii	"ft5336_ts_drv\000"
.LASF223:
	.ascii	"__USFRACT_IBIT__ 0\000"
.LASF65:
	.ascii	"__UINT_FAST32_TYPE__ unsigned int\000"
.LASF621:
	.ascii	"currActiveTouchIdx\000"
.LASF433:
	.ascii	"INT_FAST32_MIN INT32_MIN\000"
.LASF192:
	.ascii	"__LDBL_DENORM_MIN__ 1.1\000"
.LASF34:
	.ascii	"__SIZE_TYPE__ unsigned int\000"
.LASF588:
	.ascii	"FT5336_G_MODE_INTERRUPT_POLLING ((uint8_t)0x00)\000"
.LASF484:
	.ascii	"FT5336_GEST_ID_DOUBLE_CLICK ((uint8_t)0x22)\000"
.LASF292:
	.ascii	"__ULLACCUM_FBIT__ 32\000"
.LASF450:
	.ascii	"UINT8_C(x) (x ##U)\000"
.LASF530:
	.ascii	"FT5336_P5_XH_REG ((uint8_t)0x1B)\000"
.LASF458:
	.ascii	"UINTMAX_C(x) (x ##ULL)\000"
.LASF475:
	.ascii	"FT5336_DEV_MODE_MASK ((uint8_t)0x07)\000"
.LASF401:
	.ascii	"USE_USB_HS 1\000"
.LASF347:
	.ascii	"__GCC_ATOMIC_SHORT_LOCK_FREE 2\000"
.LASF8:
	.ascii	"__VERSION__ \"6.3.1 20170620 (release) [ARM/embedde"
	.ascii	"d-6-branch revision 249437]\"\000"
.LASF457:
	.ascii	"INTMAX_C(x) (x ##LL)\000"
.LASF156:
	.ascii	"__FLT_MIN_10_EXP__ (-37)\000"
.LASF647:
	.ascii	"tsStart\000"
.LASF620:
	.ascii	"currActiveTouchNb\000"
.LASF103:
	.ascii	"__WCHAR_MIN__ 0U\000"
.LASF180:
	.ascii	"__DBL_HAS_INFINITY__ 1\000"
.LASF586:
	.ascii	"FT5336_G_MODE_INTERRUPT_MASK ((uint8_t)0x03)\000"
.LASF358:
	.ascii	"__ARM_FEATURE_QBIT 1\000"
.LASF61:
	.ascii	"__INT_FAST32_TYPE__ int\000"
.LASF474:
	.ascii	"FT5336_DEV_MODE_FACTORY ((uint8_t)0x04)\000"
.LASF646:
	.ascii	"nbTouch\000"
.LASF166:
	.ascii	"__FLT_HAS_QUIET_NAN__ 1\000"
.LASF617:
	.ascii	"13TS_DrvTypeDef\000"
.LASF38:
	.ascii	"__INTMAX_TYPE__ long long int\000"
.LASF109:
	.ascii	"__INTMAX_C(c) c ## LL\000"
.LASF627:
	.ascii	"ft5336_TS_GetGestureID\000"
.LASF372:
	.ascii	"__APCS_32__ 1\000"
.LASF435:
	.ascii	"INT_FAST8_MAX INT8_MAX\000"
.LASF407:
	.ascii	"UINT16_MAX 65535\000"
.LASF50:
	.ascii	"__UINT64_TYPE__ long long unsigned int\000"
.LASF417:
	.ascii	"INTMAX_MAX 9223372036854775807LL\000"
.LASF443:
	.ascii	"PTRDIFF_MIN INT32_MIN\000"
.LASF173:
	.ascii	"__DBL_MAX_10_EXP__ 308\000"
.LASF519:
	.ascii	"FT5336_P3_XL_REG ((uint8_t)0x10)\000"
.LASF572:
	.ascii	"FT5336_CTRL_KEEP_AUTO_SWITCH_MONITOR_MODE ((uint8_t"
	.ascii	")0x01\000"
.LASF348:
	.ascii	"__GCC_ATOMIC_INT_LOCK_FREE 2\000"
.LASF601:
	.ascii	"short unsigned int\000"
.LASF343:
	.ascii	"__GCC_ATOMIC_CHAR_LOCK_FREE 2\000"
.LASF324:
	.ascii	"__TA_IBIT__ 64\000"
.LASF297:
	.ascii	"__QQ_FBIT__ 7\000"
.LASF550:
	.ascii	"FT5336_P8_YH_REG ((uint8_t)0x2F)\000"
.LASF56:
	.ascii	"__UINT_LEAST16_TYPE__ short unsigned int\000"
.LASF523:
	.ascii	"FT5336_P3_MISC_REG ((uint8_t)0x14)\000"
.LASF547:
	.ascii	"FT5336_P7_MISC_REG ((uint8_t)0x2C)\000"
.LASF155:
	.ascii	"__FLT_MIN_EXP__ (-125)\000"
.LASF400:
	.ascii	"USE_STM32746G_DISCO 1\000"
.LASF543:
	.ascii	"FT5336_P7_XL_REG ((uint8_t)0x28)\000"
.LASF637:
	.ascii	"tsEnableIT\000"
.LASF333:
	.ascii	"__REGISTER_PREFIX__ \000"
.LASF188:
	.ascii	"__DECIMAL_DIG__ 17\000"
.LASF379:
	.ascii	"__ARM_FP 4\000"
.LASF127:
	.ascii	"__INT32_C(c) c ## L\000"
.LASF613:
	.ascii	"ClearIT\000"
.LASF89:
	.ascii	"__cpp_variadic_templates 200704\000"
.LASF428:
	.ascii	"UINT_LEAST16_MAX UINT16_MAX\000"
.LASF147:
	.ascii	"__UINTPTR_MAX__ 0xffffffffU\000"
.LASF179:
	.ascii	"__DBL_HAS_DENORM__ 1\000"
.LASF515:
	.ascii	"FT5336_P2_YL_REG ((uint8_t)0x0C)\000"
.LASF332:
	.ascii	"__UTA_IBIT__ 64\000"
.LASF476:
	.ascii	"FT5336_DEV_MODE_SHIFT ((uint8_t)0x04)\000"
.LASF359:
	.ascii	"__ARM_FEATURE_SAT 1\000"
.LASF240:
	.ascii	"__LFRACT_MAX__ 0X7FFFFFFFP-31LR\000"
.LASF329:
	.ascii	"__UDA_FBIT__ 32\000"
.LASF290:
	.ascii	"__LLACCUM_MAX__ 0X7FFFFFFFFFFFFFFFP-31LLK\000"
.LASF460:
	.ascii	"WCHAR_MAX 2147483647L\000"
.LASF126:
	.ascii	"__INT_LEAST32_MAX__ 0x7fffffffL\000"
.LASF189:
	.ascii	"__LDBL_MAX__ 1.1\000"
.LASF206:
	.ascii	"__DEC64_MIN__ 1E-383DD\000"
.LASF582:
	.ascii	"FT5336_LIB_VER_H_REG ((uint8_t)0xA1)\000"
.LASF110:
	.ascii	"__UINTMAX_MAX__ 0xffffffffffffffffULL\000"
.LASF42:
	.ascii	"__SIG_ATOMIC_TYPE__ int\000"
.LASF445:
	.ascii	"SIZE_MAX INT32_MAX\000"
.LASF249:
	.ascii	"__LLFRACT_MIN__ (-0.5LLR-0.5LLR)\000"
.LASF463:
	.ascii	"FT5336_ASSERT(__condition__) do { if(__condition__)"
	.ascii	" { ; } } while(0)\000"
.LASF378:
	.ascii	"__VFP_FP__ 1\000"
.LASF402:
	.ascii	"__TS_H \000"
.LASF380:
	.ascii	"__ARM_FEATURE_FMA 1\000"
.LASF256:
	.ascii	"__ULLFRACT_EPSILON__ 0x1P-64ULLR\000"
.LASF193:
	.ascii	"__LDBL_HAS_DENORM__ 1\000"
.LASF132:
	.ascii	"__UINT_LEAST16_MAX__ 0xffff\000"
.LASF296:
	.ascii	"__ULLACCUM_EPSILON__ 0x1P-32ULLK\000"
.LASF203:
	.ascii	"__DEC64_MANT_DIG__ 16\000"
.LASF164:
	.ascii	"__FLT_HAS_DENORM__ 1\000"
.LASF577:
	.ascii	"FT5336_OFFSET_LEFT_RIGHT_REG ((uint8_t)0x92)\000"
.LASF1:
	.ascii	"__cplusplus 201103L\000"
	.ident	"GCC: (GNU) 6.3.1 20170620 (release) [ARM/embedded-6-branch revision 249437]"
