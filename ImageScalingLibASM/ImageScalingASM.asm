.code
;pixels in RCX
;newPixels in RDX
;oldWidth in R8
;newWidth in R9

scaleImage proc pixels: PTR BYTE, newPixels: PTR BYTE, oldWidth: DWORD, newWidth: DWORD, x_ratio: REAL8, y_ratio: REAL8, partSize: DWORD, totalSize: DWORD
 xor eax, eax;
 movd xmm0, x_ratio;
 movd xmm1, y_ratio;
 mov ebx, DWORD PTR[rsp + 56]; partSize
 mov R11D, DWORD PTR[rsp + 64]; totalSize

 mov esi, '0';
 mov edi, '3';

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

;py = floor(row * y_ratio);
movd xmm3, R13D;
mulps xmm3, xmm1;
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

ret;
scaleImage endp
end ; End of ASM file