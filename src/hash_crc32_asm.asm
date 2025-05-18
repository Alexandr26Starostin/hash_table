;---------------------------------------------------------------------------------------
;program on Linux-nasm-64
;has code with hash_crc32_asm                  
;---------------------------------------------------------------------------------------
section .text   ;has text with program
global hash_crc32_asm  ;global func: other files can see this func (for ld) 

;---------------------------------------------------------------------------------------------------------
;                                       hash_crc32_asm 
;calculate hash crc32 for 'char[32] str' and return it
;
;entry: rdi = ptr on str with len 32 bytes
;
;exit:  rax = remainder (hash) crc32
;
;destr: rax = result
;		rsi = mask_for: rax % 1024
;	
;must save:    rbp, rbx, r12-15
;mustn't save: others registers
;---------------------------------------------------------------------------------------------------------
hash_crc32_asm:

	xor eax, eax    ;rax = 0 - result

	crc32 rax, qword [rdi]        ;rdx = hash for 1 part element 
	crc32 rax, qword [rdi + 8]    ;rdx = hash for 2 part element 
	crc32 rax, qword [rdi + 16]   ;rdx = hash for 3 part element 
	crc32 rax, qword [rdi + 24]   ;rdx = hash for 4 part element 

	mov rsi, 01111111111b   ;rsi = 1024 - 1 = mask 
	and rax, rsi           ;rax = rax_old % 1024WWWWWW

	ret  ;rax = hash from crc32
;----------------------------------------------------------------------------------------------------------
