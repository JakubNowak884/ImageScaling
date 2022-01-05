.code
;pixels in RCX
;newPixels in RDX
;oldWidth in R8
;newWidth in R9

scaleImage proc

 push rbp
 push rbx
 push rdx
 push rcx
 push r14
 push r13
 push r12
 push r11
 push r10
 push r9
 push r8

 mov R10, RCX; pixels in R10
 mov R11, RDX; newPixels in R11
 mov R12D, DWORD PTR[rsp + 56 + 88]; partSize in R12D
 mov R13D, DWORD PTR[rsp + 64 + 88]; totalSize in R13D

 xor ebx, ebx;
 xor ecx, ecx;

 mov ebx, 3; three value in ebx
 mov ecx, 0; counter

 movsd xmm2, xmm0;
 PSLLDQ xmm0, 8;
 movsd xmm0, xmm2;

 movsd xmm2, xmm1;
 PSLLDQ xmm1, 8;
 movsd xmm1, xmm2;

 cvtsi2sd xmm8, ebx;
 PSLLDQ xmm8, 8;
 cvtsi2sd xmm8, ebx;

 cvtsi2sd xmm9, R8;
 PSLLDQ xmm9, 8;
 cvtsi2sd xmm9, R8;

;four three values in xmm5

 cvtsi2ss xmm5, ebx;
 insertps xmm5, xmm5, 16;
 insertps xmm5, xmm5, 32;
 insertps xmm5, xmm5, 48;

;(newWidth / 3) value in R9D

 mov eax, R9D;
 xor edx, edx;
 div ebx;
 mov R9D, eax;

;four (newWidth / 3) values in xmm6

 cvtsi2ss xmm6, R9D;
 insertps xmm6, xmm6, 0;
 insertps xmm6, xmm6, 16;
 insertps xmm6, xmm6, 32;
 insertps xmm6, xmm6, 48;

 M_LOOP:

;int currentSize = (totalSize + j) / 3;
;next 4 currentSizes in xmm4

 mov eax, R13D;

 add eax, ecx;
 cvtsi2ss xmm7, eax;
 insertps xmm4, xmm7, 0;

 addss xmm7, xmm5;
 insertps xmm4, xmm7, 16;

 addss xmm7, xmm5;
 insertps xmm4, xmm7, 32;

 addss xmm7, xmm5;
 insertps xmm4, xmm7, 48;

 divps xmm4, xmm5;
 roundps xmm4, xmm4, 9

;int row = (currentSize / (newWidth / 3));
;next 4 rows in xmm2

 movaps xmm2, xmm4
 divps xmm2, xmm6;
 roundps xmm2, xmm2, 9

;int column = (currentSize % (newWidth / 3));
;next 4 columns in xmm3

 xor edx, edx;
 xor eax, eax;

 cvttss2si eax, xmm4;
 div R9D;
 cvtsi2ss xmm7, edx;
 insertps xmm3, xmm7, 0;

 xor edx, edx;
 xor eax, eax;

 PSRLDQ xmm4, 4;
 cvttss2si eax, xmm4;
 div R9D;
 cvtsi2ss xmm7, edx;
 insertps xmm3, xmm7, 16;

 xor edx, edx;
 xor eax, eax;

 PSRLDQ xmm4, 4;
 cvttss2si eax, xmm4;
 div R9D;
 cvtsi2ss xmm7, edx;
 insertps xmm3, xmm7, 32;

 xor edx, edx;
 xor eax, eax;

 PSRLDQ xmm4, 4;
 cvttss2si eax, xmm4;
 div R9D;
 cvtsi2ss xmm7, edx;
 insertps xmm3, xmm7, 48;

 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;px = floor(column * x_ratio);
;next four px in xmm10 and xmm11
 cvtps2pd xmm10, xmm3;
 PSRLDQ xmm3, 8;
 cvtps2pd xmm11, xmm3;
 mulpd xmm10, xmm0;
 mulpd xmm11, xmm0;
 roundpd xmm10, xmm10, 9;
 roundpd xmm11, xmm11, 9;

;py = floor(row * y_ratio);
;next four py in xmm12 and xmm13
 cvtps2pd xmm12, xmm2;
 PSRLDQ xmm2, 8;
 cvtps2pd xmm13, xmm2;
 mulpd xmm12, xmm1;
 mulpd xmm13, xmm1;
 roundpd xmm12, xmm12, 9;
 roundpd xmm13, xmm13, 9;

;px *= 3;
 mulpd xmm10, xmm8;
 mulpd xmm11, xmm8;

;newPixels[totalSize + j] = pixels[(int)((py * oldWidth) + px)];

 mulpd xmm12, xmm9;
 mulpd xmm13, xmm9;
 addpd xmm12, xmm10;
 addpd xmm13, xmm11;
 roundpd xmm12, xmm12, 9;
 roundpd xmm13, xmm13, 9;

 cvttsd2si rdx, xmm12;
 add rdx, r10;
 mov al, BYTE PTR [rdx];
 mov R14, R13;
 add R14, R11;
 add R14, rcx;
 mov BYTE PTR [R14], al;
 
 add ecx, 1;
 cmp ecx, R12D;
 JE END_OF_LOOP

 add rdx, 1;
 mov al, BYTE PTR [rdx];
 add R14, 1;
 mov BYTE PTR [R14], al;

 add ecx, 1;
 cmp ecx, R12D;
 JE END_OF_LOOP

 add rdx, 1;
 mov al, BYTE PTR [rdx];
 add R14, 1;
 mov BYTE PTR [R14], al;

 add ecx, 1;
 cmp ecx, R12D;
 JE END_OF_LOOP

 PSRLDQ xmm12, 8;
 cvttsd2si rdx, xmm12;
 add rdx, r10;
 mov al, BYTE PTR [rdx];
 add R14, 1;
 mov BYTE PTR [R14], al;

 add ecx, 1;
 cmp ecx, R12D;
 JE END_OF_LOOP

 add rdx, 1;
 mov al, BYTE PTR [rdx];
 add R14, 1;
 mov BYTE PTR [R14], al;

 add ecx, 1;
 cmp ecx, R12D;
 JE END_OF_LOOP

 add rdx, 1;
 mov al, BYTE PTR [rdx];
 add R14, 1;
 mov BYTE PTR [R14], al;

 add ecx, 1;
 cmp ecx, R12D;
 JE END_OF_LOOP

 cvttsd2si rdx, xmm13;
 add rdx, r10;
 mov al, BYTE PTR [rdx];
 add R14, 1;
 mov BYTE PTR [R14], al;

 add ecx, 1;
 cmp ecx, R12D;
 JE END_OF_LOOP

 add rdx, 1;
 mov al, BYTE PTR [rdx];
 add R14, 1;
 mov BYTE PTR [R14], al;

 add ecx, 1;
 cmp ecx, R12D;
 JE END_OF_LOOP

 add rdx, 1;
 mov al, BYTE PTR [rdx];
 add R14, 1;
 mov BYTE PTR [R14], al;

 add ecx, 1;
 cmp ecx, R12D;
 JE END_OF_LOOP

 PSRLDQ xmm13, 8;
 cvttsd2si rdx, xmm13;
 add rdx, r10;
 mov al, BYTE PTR [rdx];
 add R14, 1;
 mov BYTE PTR [R14], al;

 add ecx, 1;
 cmp ecx, R12D;
 JE END_OF_LOOP

 add rdx, 1;
 mov al, BYTE PTR [rdx];
 add R14, 1;
 mov BYTE PTR [R14], al;

 add ecx, 1;
 cmp ecx, R12D;
 JE END_OF_LOOP

 add rdx, 1;
 mov al, BYTE PTR [rdx];
 add R14, 1;
 mov BYTE PTR [R14], al;

 add ecx, 1;
 cmp ecx, R12D;
 JNE M_LOOP
 END_OF_LOOP:

pop r8
pop r9
pop r10
pop r11
pop r12
pop r13
pop r14
pop rcx
pop rdx
pop rbx
pop rbp

ret

scaleImage endp
end ; End of ASM file