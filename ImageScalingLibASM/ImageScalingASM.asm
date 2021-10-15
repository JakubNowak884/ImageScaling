;*******************************************************************************
;* Laboratory 1 simple assembly procedure call *
;* *
;* Standard Windows memory model *
;*******************************************************************************
;model flat, stdcall
.code
;*******************************************************************************
;* *
;* Assembler procedure MyProc1 changes Flags register *
;* *
;* Input: x: DWORD (C++ int type), y: DWORD (C++ int type) *
;* Output: z: DWORD (C++ int type) in the EAX register *
;* *
;*******************************************************************************
scaleImage proc x: DWORD, y: DWORD
 xor eax,eax ; EAX = 0
 mov eax,x ; Param1 eax = x
 mov ecx,y ; Param2 ecx = y
 ror ecx,1 ; shift ecx right by 1
 shld eax,ecx,2 ; set flags registry
 jnc ET1
 mul y
 ret ; return z in EAX register
 ET1: mul x
 neg y
 ret ; return z in EAX register
scaleImage endp
end ; End of ASM file