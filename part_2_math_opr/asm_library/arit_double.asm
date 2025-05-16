section .text
    global tambah_double
    global kurang_double
    global kali_double
    global bagi_double

;fungsi tambah untuk angka pecahan
;fungsi tambah_double(double a, double b)
tambah_double:
    addsd xmm0, xmm1   ; xmm0 = xmm0 + xmm1 (double)
    ret                ; hasil di xmm0 

;fungsi kurang untuk angka pecahan
kurang_double:
    subsd xmm0, xmm1   ; xmm0 = xmm0 - xmm1 (double)
    ret                ; hasil di xmm0

; fungsi kali_double(double a, double b)
kali_double:
    mulsd xmm0, xmm1
    ret

; fungsi bagi_double(double a, double b)
bagi_double:
    divsd xmm0, xmm1
    ret         