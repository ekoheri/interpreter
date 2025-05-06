section .data
    msg db "Halo dari fungsi cetak!", 0

section .text
    global _start

_start:
    ; cetak(msg)
    mov     rdi, msg       ; argumen: alamat string
    call    cetak

    ; keluar dari program
    mov     rax, 60        ; syscall exit
    xor     rdi, rdi       ; exit code 0
    syscall

; ================= dibawah Ini =======
; adalah kumpulan fungsi (library)
; void cetak(char *buffer)
cetak:
    mov     rsi, rdi       ; rsi = buffer
    mov     rax, 1         ; syscall write
    mov     rdi, 1         ; stdout
    mov     rdx, 0         ; panjang awal

.find_len:
    cmp     byte [rsi + rdx], 0
    je      .done_len
    inc     rdx
    jmp     .find_len

.done_len:
    syscall
    ret
