.code
;pixels in RCX
;newPixels in RDX
;oldWidth in R8
;newWidth in R9

scaleImage proc pixels: PTR BYTE, newPixels: PTR BYTE, oldWidth: DWORD, newWidth: DWORD, x_ratio: REAL8, y_ratio: REAL8, partSize: DWORD, totalSize: DWORD
 xor eax, eax;
 movsd xmm0, x_ratio;
 movsd xmm1, y_ratio;
 mov ebx, DWORD PTR[rsp + 56]; partSize
 mov R11D, DWORD PTR[rsp + 64]; totalSize

 mov esi, '0';
 mov edi, '3';

 xor edx, edx;
 xor eax, eax;

 M_LOOP:
 ;(newWidth / 3)
 mov eax, R9D;
div edi;
mov R9D, eax;

;int currentSize = (totalSize + j) / 3;
add R10D, esi;
mov eax, R10d;
div edi;
mov R12D, eax;
;int row = (currentSize / (newWidth / 3));
mov eax, R12D;
div R9D;
mov R13D, eax;
;int column = (currentSize % (newWidth / 3));
mov eax, R12D;
div  edi;
mov R12D, edx;
;px = floor(column * x_ratio);
movd xmm2, R12D;
mulps xmm2, xmm0;
roundps xmm2, xmm2, 9;

;py = floor(row * y_ratio);
cvtsi2ss R13D, xmm3 ;nie dzia³a, coœ nie tak z miejscem
mulps xmm3, xmm1;
<<<<<<< HEAD
roundps xmm3, xmm3, 9;

;px *= 3;
movd xmm0, edi;
mulps xmm2, xmm0;

;newPixels[totalSize + j] = pixels[(int)((py * oldWidth) + px)];
movd xmm0, R8D;
mulsd xmm3, xmm0;
addsd xmm3, xmm2;
roundps xmm3, xmm3, 9;
cvttsd2si rsi, xmm3;
add rdi, RCX;
add rsi, RDX;
mov al, BYTE PTR [rsi];
mov BYTE PTR [rdi], al;

;newPixels[totalSize + j + 1] = pixels[(int)((py * oldWidth) + px) + 1];
mov rdi, '1';
add R11D, edi;
movd xmm0, edi;
addsd xmm3, xmm0;
cvttsd2si edi, xmm3
add rdi, RCX
add rsi, RDX;
mov al, BYTE PTR [rsi]
mov BYTE PTR [rdi], al

;newPixels[totalSize + j + 2] = pixels[(int)((py * oldWidth) + px) + 2];
mov edi, '2';
add R11D, edi;
movd xmm0, edi;
addsd xmm3, xmm0;
cvttsd2si edi, xmm3
add rdi, RCX
add rsi, RDX;
mov al, BYTE PTR  [rsi]
mov BYTE PTR  [rdi], al

mov eax, '3';
sub ebx, eax;
mov eax, '0'
cmp ebx, eax;
JE M_LOOP
=======
roundps xmm3, xmm3;

;px *= 3;
mulps xmm2, edi;

;newPixels[totalSize + j] = pixels[(int)((py * oldWidth) + px)];
mulps xmm3, R8;
addps xmm3, xmm2;
roundps xmm4, xmm3;

;newPixels[totalSize + j + 1] = pixels[(int)((py * oldWidth) + px) + 1];
mov edi, '1';
addps xmm3, edi;

;newPixels[totalSize + j + 2] = pixels[(int)((py * oldWidth) + px) + 2];
addps xmm3, edi;
>>>>>>> d82ad5a3fadbd2b18b794ac6df3582584ecad036

ret;
scaleImage endp
end ; End of ASM file