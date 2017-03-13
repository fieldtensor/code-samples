;
; +-----------------------------------+
; |       SHUFPS 0b DD CC BB AA       |
; +--------+--------+--------+--------+
; |   D3   |   D2   |   D1   |   D0   |
; +--------+--------+--------+--------+
; | SRC DD | SRC CC | DST BB | DST AA | 
; +--------+--------+--------+--------+
;

%macro Method 1
global %1
%1:
%endmacro

section .text

; Constructor
Method \
__ZN2SK5Vec3TIdLm32EEC1Eddd
    ; xmm0[0] = xmm0[0] (x)
    ; xmm0[1] = xmm1[0] (y)
    shufpd xmm0, xmm1, 0x02 ; 00 00 00 10

    ; rdi[0] = xmm0[0] (x)
    ; rdi[1] = xmm0[1] (y)
    movapd [rdi], xmm0

    ; rdi[2] = xmm2[2] (z)
    movsd [rdi+0x10], xmm2

    ret

; operator+
Method \
__ZNK2SK5Vec3TIdLm32EEplERKS1_
    movapd xmm0, [rdi+0x00]
    movsd xmm1, [rdi+0x10]

    movapd xmm2, [rsi+0x00]
    movsd xmm3, [rsi+0x10]

    addpd xmm0, xmm2
    addsd xmm1, xmm3

    movpd xmm2, xmm1

    shufpd xmm1, [rsi]
    addps xmm0, xmm1

    ; xmm1[0] = xmm0.z
    movhlps xmm1, xmm0

    ret

; operator-
Method \
__ZNK2SK5Vec3TIdLm32EEmiERKS1_
    movaps xmm0, [rdi]
    movaps xmm1, [rsi]
    subps xmm0, xmm1

    ; xmm1[0] = xmm0.z
    movhlps xmm1, xmm0

    ret

; operator* (scalar)
Method \
__ZNK2SK5Vec3TIdLm32EEmlEf
    shufps xmm0, xmm0, 0x00
    movaps xmm1, [rdi]
    mulps xmm0, xmm1

    ; xmm1[0] = xmm0.z
    movhlps xmm1, xmm0

    ret

; operator/ (scalar)
Method \
__ZNK2SK5Vec3TIdLm32EEdvEf
    shufps xmm0, xmm0, 0x00
    movaps xmm1, [rdi]
    divps xmm1, xmm0

    ; xmm0[0] = xmm1.x
    ; xmm0[1] = xmm1.y
    movaps xmm0, xmm1

    ; xmm1[0] = xmm1.z
    movhlps xmm1, xmm1

    ret

; operator*=
Method \
__ZN2SK5Vec3TIdLm32EEmLEf
shufps xmm0, xmm0, 0x00
    movaps xmm1, [rdi]
    mulps xmm1, xmm0
    movaps [rdi], xmm1
    mov rax, rdi
    ret

; operator/=
Method \
__ZN2SK5Vec3TIdLm32EEdVEf
    shufps xmm0, xmm0, 0x00
    movaps xmm1, [rdi]
    divps xmm1, xmm0
    movaps [rdi], xmm1
    mov rax, rdi
    ret

; operator* (dot product)
Method \
__ZNK2SK5Vec3TIdLm32EEmlERKS1_
    movaps xmm0, [rdi]
    mulps xmm0, [rsi]

    ; xmm1[2] = xmm0.x
    shufps xmm1, xmm0, 0x00

    ; xmm2[2] = xmm0.y
    shufps xmm2, xmm0, 0x10

    ; xmm0[2] += xmm1[2]
    addps xmm0, xmm1

    ; xmm0[2] += xmm2[2]
    addps xmm0, xmm2

    movhlps xmm0, xmm0
    ret

; Cross Product
Method \
__ZNK2SK5Vec3TIdLm32EE5crossERKS1_
    movaps xmm4, [rdi]
    movaps xmm5, [rsi]

    ; a := xmm4
    ; b := xmm5

    ; xmm2 = [a.x, a.z, a.y]
    movaps xmm0, xmm4
    shufps xmm0, xmm0, 0x09 ; 00 00 10 01

    ; xmm3 = [b.y, b.x, b.z]
    movaps xmm1, xmm5
    shufps xmm1, xmm1, 0x12 ; 00 01 00 10

    ; xmm4 = [a.y, a.x, a.z]
    movaps xmm2, xmm4
    shufps xmm2, xmm2, 0x12 ; 00 01 00 10

    ; xmm5 = [b.x, b.z, b.y]
    movaps xmm3, xmm5
    shufps xmm3, xmm3, 0x09 ; 00 00 10 01

    mulps xmm0, xmm1 ; xmm2 = [a.x*b.y, a.z*b.x, a.y*b.z]
    mulps xmm2, xmm3 ; xmm4 = [a.y*b.x, a.x*b.z, a.z*b.y]

    subps xmm0, xmm2

    movhlps xmm1, xmm0
    ret

Method \
__ZNK2SK5Vec3TIdLm32EE7length2Ev
    mov rsi, rdi
    ; dot product
    call __ZNK2SK5Vec3TIdLm32EEmlERKS1_
    ret

Method \
__ZNK2SK5Vec3TIdLm32EE6lengthEv
    mov rsi, rdi
    ; dot product
    call __ZNK2SK5Vec3TIdLm32EEmlERKS1_
    sqrtss xmm0, xmm0
    ret

Method \
__ZN2SK5Vec3TIdLm32EE9normalizeEv
    ; length()
    call __ZNK2SK5Vec3TIdLm32EE6lengthEv 
    ; operator /=
    call __ZN2SK5Vec3TIdLm32EEdVEf 
    ret

Method \
__ZNK2SK5Vec3TIdLm32EE6normalEv
    ; Load the vector
    movaps xmm2, [rdi]

    ; Save a clean copy for later
    movaps xmm0, xmm2

    ; Start the dot product computation
    mulps xmm2, xmm2

    ; xmm3[2] = xmm2.x
    shufps xmm3, xmm2, 0x00

    ; xmm3[2] = xmm2.y
    shufps xmm4, xmm2, 0x10

    ; xmm2[2] += xmm1[2]
    addps xmm2, xmm3

    ; xmm2[2] += xmm2[2]
    addps xmm2, xmm3

    ; xmm2[0] = xmm2[2]
    movhlps xmm2, xmm2

    sqrtss xmm2, xmm2

    ; Broadcast xmm2[0]
    shufps xmm2, xmm2, 0x00

    ; Recall that xmm0 holds a
    ; clean copy of the vector
    divps xmm0, xmm2

    movhlps xmm1, xmm0
    ret

Method \
__ZN2SK5Vec3TIdLm32EEixEm
    mov rax, rsi
    shl rax, 2
    add rax, rdi
    ret

Method \
__ZNK2SK5Vec3TIdLm32EEixEm
    movss xmm0, [rdi+4*rsi]
    ret

Method \
__ZN2SK5Vec3TIdLm32EEcvPfEv
    mov rax, rdi
    ret    

Method \
__ZNK2SK5Vec3TIdLm32EEcvPKfEv
    mov rax, rdi
    ret
