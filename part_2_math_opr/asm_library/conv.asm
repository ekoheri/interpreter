section .data
    tempval dq 0      ; 64-bit (double)
    tempval2 dq 0     ; 64-bit (double)
    tempval3 dq 0     ; 64-bit (double)

section .text
    global string_to_int
    global string_to_double

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

string_to_double:
    push rbx
    push rsi
    push rdi

    mov rsi, rdi          ; rsi = pointer ke string input
    xor rax, rax          ; integer part = 0
    xor rbx, rbx          ; flag titik desimal
    xor rcx, rcx          ; digit pecahan
    xor rdx, rdx          ; pecahan angka

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
    inc rsi
    jmp .next_char

.int_part:
    imul rax, rax, 10
    add rax, r8
    inc rsi
    jmp .next_char

.handle_dot:
    mov rbx, 1
    inc rsi
    jmp .next_char

.done_parsing:
    ; konversi integer part ke double (xmm0)
    cvtsi2sd xmm0, rax

    ; simpan pecahan dan digit ke memori
    mov     [rel tempval], rdx
    mov     [rel tempval2], rcx

    ; buat pembagi 10^rcx
    mov     rax, 1
    mov     r9, rcx
    cmp     r9, 0
    je .skip_pow

.pow_loop:
    imul rax, rax, 10
    dec r9
    jnz .pow_loop

.skip_pow:
    mov     [rel tempval3], rax

    ; konversi pecahan dan pembagi ke double
    movsd   xmm1, [rel tempval]    ; pecahan
    movsd   xmm2, [rel tempval3]   ; pembagi

    divsd   xmm1, xmm2
    addsd   xmm0, xmm1             ; total = int + pecahan

    pop rdi
    pop rsi
    pop rbx
    ret        