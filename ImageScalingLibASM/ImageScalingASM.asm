.code
;pixels in RCX
;newPixels in RDX
;oldWidth in R8
;newWidth in R9

scaleImage proc pixels

 push rbp
 push rbx
 push rsi
 push rdi
 push rdx
 push rcx
 push r15
 push r14
 push r13
 push r12
 push r11
 push r10
 push r9
 push r8

 xor eax, eax;
 xor R11, r11;
 mov ebx, DWORD PTR[rsp + 64 + 112]; partSize
 mov R11D, DWORD PTR[rsp + 72 + 112]; totalSize

 mov R14, RDX
 mov rsi, 0;
 mov rdi, 3;

 xor edx, edx;
 xor eax, eax;

;(newWidth / 3)
mov eax, R9D;
div edi;
mov R9D, eax;

 M_LOOP:

 xor edx, edx;
 xor eax, eax;
 xor rdi, rdi;
 mov rdi, 3;

;int currentSize = (totalSize + j) / 3;
add esi, r11d;
mov eax, esi;
div edi;
mov R12D, eax;
;int row = (currentSize / (newWidth / 3));
 xor edx, edx;
 xor eax, eax;

mov eax, R12D;
div R9D;
mov R13D, eax;
;int column = (currentSize % (newWidth / 3));
 xor edx, edx;
 xor eax, eax;

mov eax, R12D;
div  R9D;
mov R12D, edx;
;px = floor(column * x_ratio);
cvtsi2sd xmm2, R12D;
mulpd xmm2, xmm0;
roundpd xmm2, xmm2, 9;

;py = floor(row * y_ratio);
cvtsi2sd xmm3, R13D;
mulpd xmm3, xmm1;
roundpd xmm3, xmm3, 9;

;px *= 3;
cvtsi2sd xmm5, edi;
mulpd xmm2, xmm5;

;newPixels[totalSize + j] = pixels[(int)((py * oldWidth) + px)];
cvtsi2sd xmm5, R8D;
mulsd xmm3, xmm5;
addsd xmm3, xmm2;
roundpd xmm3, xmm3, 9;
cvttsd2si rdi, xmm3; powinno wyjœæ 1 a wychodzi 2 w drugim przejœciu pêtli

add rdi, RCX;
mov r15, rsi;
add r15, R14;
mov al, BYTE PTR [rdi];
mov BYTE PTR [r15], al;

;newPixels[totalSize + j + 1] = pixels[(int)((py * oldWidth) + px) + 1];
mov rdx, 1;
add rdi, Rdx
add r15, RDX;
mov al, BYTE PTR [rdi];
mov BYTE PTR [r15], al;

;newPixels[totalSize + j + 2] = pixels[(int)((py * oldWidth) + px) + 2];
mov rdx, 1;
add rdi, Rdx
add r15, rdx;
mov al, BYTE PTR  [rdi];
mov BYTE PTR  [r15], al;

add rsi, 3;
cmp rbx, rsi;

JNE M_LOOP

pop r8
pop r9
pop r10
pop r11
pop r12
pop r13
pop r14
pop r15
pop rcx
pop rdx
pop rdi
pop rsi
pop rbx
pop rbp

ret

scaleImage endp
end ; End of ASM file