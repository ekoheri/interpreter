; compile :
; nasm -f elf64 -g -F dwarf standardio.asm -o standardio.o
; gcc -shared -fPIC -o libraryio.so standardio.o

section .data
    tempval dq 0      ; 64-bit (double)
    tempval2 dq 0     ; 64-bit (double)
    tempval3 dq 0     ; 64-bit (double)

section .text
    global cetak
    global baca
    global panjang_string
    
    global tambah
    global kurang
    global string_to_int

    global tambah_double
    global string_to_double

; void cetak(char *buffer)
cetak:
    mov     rsi, rdi        ; rsi = buffer
    mov     rax, 1          ; syscall write
    mov     rdi, 1          ; stdout
    mov     rdx, 0
.find_len:
    cmp     byte [rsi + rdx], 0
    je      .done_len
    inc     rdx
    jmp     .find_len
.done_len:
    syscall
    ret

; void baca(char *buffer)
baca:
    mov     rsi, rdi        ; rsi = buffer
    mov     rax, 0          ; syscall read
    mov     rdi, 0          ; stdin
    mov     rdx, 99         ; max 99 chars
    syscall

    ; tambahkan null terminator
    mov     byte [rsi + rax], 0

    ; hapus newline
    mov     rcx, rax
    dec     rcx
.find_newline:
    cmp     rcx, -1
    jl      .done
    cmp     byte [rsi + rcx], 10
    jne     .done
    mov     byte [rsi + rcx], 0
.done:
    ret

; size_t panjang_string(char *str)
panjang_string:
    xor     rcx, rcx
.loop:
    cmp     byte [rdi + rcx], 0
    je      .done_strlen
    inc     rcx
    jmp     .loop
.done_strlen:
    mov     rax, rcx
    ret

;fungsi untuk mengkonversi string ke integer
string_to_int:
    xor     rax, rax        ; rax = hasil (accumulator)
    xor     rcx, rcx        ; rcx = index loop
.next_char:
    mov     bl, [rdi + rcx] ; ambil karakter
    cmp     bl, 0
    je      .done_stoi      ; jika NULL, selesai
    cmp     bl, 10
    je      .done_stoi      ; jika newline (optional)
    cmp     bl, 13
    je      .done_stoi      ; jika carriage return (optional)

    sub     bl, '0'         ; konversi ASCII ke angka
    cmp     bl, 9
    ja      .done_stoi      ; jika bukan digit, keluar
    imul    rax, rax, 10    ; kalikan hasil sebelumnya dengan 10
    add     rax, rbx        ; tambahkan digit
    inc     rcx
    jmp     .next_char

.done_stoi:
    ret

tambah:
    mov eax, edi     ; argumen pertama (a)
    add eax, esi     ; tambah dengan argumen kedua (b)
    ret    

kurang:
    mov eax, edi     ; argumen pertama (a)
    sub eax, esi     ; tambah dengan argumen kedua (b)
    ret  

; Fungsi String to Double
string_to_double:
    push rbx
    push rsi
    push rdi

    mov rsi, rdi
    xor rax, rax
    xor rbx, rbx
    xor rcx, rcx
    xor rdx, rdx

.next_char:
    movzx r8, byte [rsi]
    test r8, r8
    jz .done_parsing

    cmp r8, '.'
    je .handle_dot

    cmp r8, '0'
    jb .done_parsing
    cmp r8, '9'
    ja .done_parsing

    sub r8, '0'
    cmp rbx, 0
    je .int_part

    imul rdx, rdx, 10
    add rdx, r8
    inc rcx
    jmp .next_digit

.int_part:
    imul rax, rax, 10
    add rax, r8
    jmp .next_digit

.handle_dot:
    mov rbx, 1
    inc rsi
    jmp .next_char

.next_digit:
    inc rsi
    jmp .next_char

.done_parsing:
    mov     [rel tempval], rax     ; simpan integer part
    mov     [rel tempval2], rdx    ; simpan pecahan
    mov     [rel tempval3], rcx    ; simpan digit pecahan

    ; konversi integer part ke double di xmm0
    movsd   xmm0, [rel tempval]

    ; [optional] bisa tambah logika untuk menghitung pecahan ke double juga
    ; contoh: pecahan = rdx / (10^rcx) → butuh floating point math

    pop rdi
    pop rsi
    pop rbx
    ret

;fungsi tambah untuk angka pecahan
tambah_double:
    addsd xmm0, xmm1   ; xmm0 = xmm0 + xmm1 (double)
    ret                ; hasil di xmm0 
