section .text

global hash_crc32
;=====================================
; rdi - object
;=====================================
hash_crc32:
        mov eax, -1             ; start value
        _loop:
            mov rcx, [rdi]

            test rcx, rcx       ;
            je _ret             ; if (rcx == 0) return rax;

            crc32 rax, rcx

            inc rdi             ; rdi++
            jmp _loop           ; while(al == cl)

        _ret:
        ret