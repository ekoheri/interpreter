; compile :
; nasm -f elf64 standardio.asm -o standardio.o
; gcc -shared -fPIC -o libraryio.so standardio.o

section .text
    global cetak
    global baca
    global panjang_string
    
    global tambah
    global kurang
    global string_to_int

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
