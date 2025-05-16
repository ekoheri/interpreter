section .text
    global tambah
    global kurang
    global kali
    global bagi
    global modulus

; Fungsi tambah
tambah:
    mov eax, edi     ; argumen pertama (a)
    add eax, esi     ; tambah dengan argumen kedua (b)
    ret    

; Fungsi kurang
kurang:
    mov eax, edi     ; argumen pertama (a)
    sub eax, esi     ; tambah dengan argumen kedua (b)
    ret

kali:
    mov eax, edi
    imul eax, esi
    ret

; int bagi(int a, int b)
bagi:
    mov eax, edi       ; eax = a
    cdq                ; sign-extend EAX → EDX:EAX
    idiv esi           ; eax = eax / esi
    ret

; int modulus(int a, int b)
modulus:
    mov eax, edi     ; dividend (a)
    cdq              ; sign extend eax to edx:eax
    idiv esi         ; divide eax by esi → quotient in eax, remainder in edx
    mov eax, edx     ; return remainder
    ret   

