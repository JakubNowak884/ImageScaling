.code
scaleImage proc pixels: DWORD, newPixels: DWORD, oldWidth: DWORD, newWidth: DWORD, x_ratio: REAL8, y_ratio: REAL8, sizze: DWORD, totalSize: DWORD
 xor eax,eax ; EAX = 0
 ret;
scaleImage endp
end ; End of ASM file