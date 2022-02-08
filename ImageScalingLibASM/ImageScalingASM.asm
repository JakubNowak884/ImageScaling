;Interpolacja obrazu metod� "najbli�szego s�siada"
;Algorytm polega na wyliczaniu wsp�rz�dnych piksela, kt�ry ma zosta� skopiowany z oryginalnego obrazu w kolejne miejsce nowego obrazu
;Jakub Nowak, AEI Gliwice, informatyka, semestr V, data wykonania: 11.01.2022 r.
;aktualna wersja 1.1 - usprawnienie czytania z pami�ci (czytanie po 4 bajty)
;wersja 1.0 - skomentowanie kodu i drobne usprawnienia
;wersja 0.9 - dodanie instrukcji wektorowych
;wersja 0.8 - implementacja algorytmu na standardowych rejestrach

.code

;procedura scaleImage
;powi�ksza obrazy o liczby ca�kowite lub nieca�kowite wi�ksze lub r�wne 1
;pixels - adres pocz�tku tablicy zawieraj�cej bajty RGB wej�ciowego obrazu
;newPixels - adres pocz�tku tablicy zawieraj�cej bajty RGB wyj�ciowego obrazu
;oldWidth - szeroko�� wej�ciowego obrazu
;newWidth - szeroko�� wyj�ciowego obrazu
;x_ratio - stosunek szeroko�ci wej�ciowego obrazu do szeroko�ci wyj�ciowego (zakres od 0 do 1)
;y_ratio - stosunek wysoko�ci wej�ciowego obrazu do wysoko�ci wyj�ciowego (zakres od 0 do 1)
;partSize - ilo�� bajt�w jakie ma przetworzy� dany w�tek
;totalSize - bajt od kt�rego dany w�tek ma zacz�� przetwarzania
;znacznik ZR(zero flag) zmienia warto�� na 1
;znaczik PE(parity flag) zmienia warto�� na 1

scaleImage proc

 ;pushing all registers used in program to the stack
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

 ;arguments
 ;pixels in RCX
 ;newPixels in RDX
 ;oldWidth in R8
 ;newWidth in R9
 mov R10, RCX; pixels in R10
 mov R11, RDX; newPixels in R11
 mov R12D, DWORD PTR[rsp + 56 + 88]; partSize in R12D
 mov R13D, DWORD PTR[rsp + 64 + 88]; totalSize in R13D
 shufps xmm0, xmm0, 0; vector of four floats x_ratio value in xmm0 register
 shufps xmm1, xmm1, 0; vector of four floats y_ratio value in xmm1 register

 ;four three values in xmm5
 xor ebx, ebx; set ebx to zero
 mov ebx, 3; three value in ebx
 cvtsi2ss xmm5, ebx; put value from ebx(3) to xmm5 register as float
 shufps xmm5, xmm5, 0; vector of four floats 3 value in xmm5 register
 
 ;four oldWidth values in xmm9
 cvtsi2ss xmm9, R8; put value from R8(oldWidth) to xmm9 register as float
 shufps xmm9, xmm9, 0; vector of four floats oldWidth value in xmm9 register

 ;(newWidth / 3) value in R9D
 xor edx, edx; set edx to zero for remaider of division
 mov eax, R9D; newWidth from R9D as divident
 div ebx; newWidth / 3
 mov R9D, eax; move result of division to R9D
 cvtsi2ss xmm6, R9D; move result of division to xmm6
 shufps xmm6, xmm6, 0; vector of four floats (newWidth / 3) value in xmm6 register

 ;counter for M_LOOP
 xor ecx, ecx; set ecx to zero

 ;main loop, each iteration calculates position for 4 pixels and increases counter by 12
 M_LOOP:

 ;int currentSize = (totalSize + j) / 3;
 ;next 4 currentSizes in xmm4
 mov eax, R13D; move value from R13D(totalSize) to eax register

 add eax, ecx; add counter to value in eax register(totalSize)
 cvtsi2ss xmm7, eax; put value from eax(totalSize + j) to xmm7 register as float
 insertps xmm4, xmm7, 0; put value from xmm7(totalSize + j) as first 4 bytes of xmm4 register

 addss xmm7, xmm5; add single value from xmm5(3) to value in xmm7(totalSize + j + 3), for next pixel
 insertps xmm4, xmm7, 16; put value from xmm7(totalSize + j + 3) as next 4 bytes of xmm4 register

 addss xmm7, xmm5; add single value from xmm5(3) to value in xmm7(totalSize + j + 6), for next pixel
 insertps xmm4, xmm7, 32; put value from xmm7(totalSize + j + 6) as next 4 bytes of xmm4 register

 addss xmm7, xmm5; add single value from xmm5(3) to value in xmm7(totalSize + j + 9), for next pixel
 insertps xmm4, xmm7, 48; put value from xmm7(totalSize + j + 9) as next 4 bytes of xmm4 register

 divps xmm4, xmm5; division of vector of four float values in xmm4(four totalSizes + counters) by vector of four float values in xmm5(3)
 roundps xmm4, xmm4, 9; rounding down vector of four float values in xmm4(four currentSizes)

 ;int row = (currentSize / (newWidth / 3));
 ;next 4 rows in xmm2
 movaps xmm2, xmm4; put vector of four float values in xmm4(four currentSizes) in xmm2 register
 divps xmm2, xmm6; division of vector of four float values in xmm2(four currentSizes) by vector of four float values in xmm6(newWidth / 3)
 roundps xmm2, xmm2, 9; rounding down vector of four float values in xmm2(four rows)

 ;int column = (currentSize % (newWidth / 3));
 ;next 4 columns in xmm3

 ;counter for MODULO_LOOP
 xor R15D, R15D; set R15D to zero

 ;calculates modulo (currentSize % (newWidth / 3))
 MODULO_LOOP:

 PSRLDQ xmm3, 4; shift right 4 bytes of xmm3 register to get a 4 bytes space to insert another value as last 4 bytes
 xor edx, edx; set edx to zero for remaider of division
 cvttss2si eax, xmm4; put value from xmm4(currentSize) to eax register as integer
 PSRLDQ xmm4, 4; shift right 4 bytes of xmm4 register to get access to next currentSize value
 div R9D; currentSize / (newWidth / 3)
 cvtsi2ss xmm7, edx; put remainder from divison to xmm7 register as float
 insertps xmm3, xmm7, 48; put reaminder as last 4 bytes of xmm3 register
 
 add R15D, 1; add 1 to modulo loop counter
 cmp R15D, 4; check if four iterations of modulo loop has been passed
 JNE MODULO_LOOP; if it isn't fourth iteration jump and the start of modulo loop

 ;px = floor(column * x_ratio);
 ;next four px in xmm3
 mulps xmm3, xmm0; multiplication of vector of four float values in xmm3(four columns) by vector of four float values in xmm0(x_ratio)
 roundps xmm3, xmm3, 9; rounding down vector of four float values in xmm3(four px)

 ;py = floor(row * y_ratio);
 ;next four py in xmm2
 mulps xmm2, xmm1; multiplication of vector of four float values in xmm2(four rows) by vector of four float values in xmm1(y_ratio)
 roundps xmm2, xmm2, 9; rounding down vector of four float values in xmm2(four py)

 ;px *= 3;
 mulps xmm3, xmm5; multiplication of vector of four float values in xmm3(four px) by vector of four float values in xmm5(3)

 ;newPixels[totalSize + j] = pixels[(int)((py * oldWidth) + px)];
 mulps xmm2, xmm9; multiplication of vector of four float values in xmm2(four py) by vector of four float values in xmm9(oldWidth)
 addps xmm2, xmm3; addition of vector of four float values in xmm2(four py * oldWidth) by vector of four float values in xmm3(four px)
 roundps xmm2, xmm2, 9; rounding down vector of four float values in xmm2((py * oldWidth) + px)

 mov R14, R13; move value from R13(totalSize) to R14 register
 add R14, R11; addition of value from R14(totalSize) and R11(beggining of newPixels array)

 ;counter for READ_LOOP
 xor R15D, R15D; set R15D to zero

 ;reads pixel in each iteration from pixel array and puts in into newPixel array
 READ_LOOP:

 cvttss2si rdx, xmm2; put value from xmm2(position of pixel that will be copied) to rdx register as integer
 PSRLDQ xmm2, 4; shift right 4 bytes of xmm2 register to get access to next postion of pixel
 mov eax, DWORD PTR [R10 + rdx]; move pixel from pixel array(last one is B value from next pixel and will be replaced in next iteration) to eax
 mov DWORD PTR [R14 + rcx], eax; move pixel from eax to newPixel arrray
 
 add ecx, 3; add 3 to counter
 cmp ecx, R12D; check if counter is equal to R12(partSize)
 JE END_OF_LOOP; if equal jump to end of main loop
 
 add R15D, 1; add 1 to read loop counter
 cmp R15D, 4; check if four iterations of read loop has been passed
 JNE READ_LOOP; if it isn't fourth iteration jump at the start of read loop
 
 JMP M_LOOP; jump and the start of main loop
 
 ;end of main loop
 END_OF_LOOP:

 ;poping all registers used in program from the stack
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