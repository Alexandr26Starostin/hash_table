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
;		rcx = count iterations for count crc32
;		rsi = index element in str 
;			= COUNT_BUCKETS
;           = mask_for: rax % 1024
;		rdx = hash from every element 
;	
;must save:    rbp, rbx, r12-15
;mustn't save: others registers
;---------------------------------------------------------------------------------------------------------
hash_crc32_asm:

	xor eax, eax    ;rax = 0 - result
	mov rcx, 4d     ;rcx = 4 - count iterations for count crc32            
	xor esi, esi    ;rsi = 0 - index on element in str

	count_crc32_for_new_elem:

		crc32 rdx, qword [rdi + rsi]   ;rdx = hash for element 

		add rsi, 8d  ;rsi += 8 (skip old elem with len 32 bytes)
		add rax, rdx ;rax += rdx (save hash crc32 for element) 

	loop count_crc32_for_new_elem     ;while (rcx != 0) {count_crc32_for_new_elem ();}  //while have elements in str, continue count crc32

	mov rsi, 01111111111b   ;rsi = 1024 - 1 = mask 
	and rax, rsi           ;rax = rax_old % 1024

	;mov rsi, 727d ;const size_t COUNT_BUCKETS = 727;
	;;cqo           ;rax -> rdx:rax
	;xor edx, edx
	;div rsi       
				  ;rax = rax_old / 727
				  ;rdx = rax_old % 727

	;mov rax, rdx

	ret  ;rax = remainder (hash) from crc32
;----------------------------------------------------------------------------------------------------------
