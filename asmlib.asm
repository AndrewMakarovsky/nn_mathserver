global movsdq
global movsdd
global movsdw
global movsdb
global matrix8tran
global v8gsum
global v8add
global v8mult
global fill8arr
global forwbs

section .text

;� Linux ����������� ��������� ABI ��� ���������� "System V ABI".
;�������� ����������� ������ 6 ���������� ���������� � ������� ����� ��������� �������� (� ������� ���������� ����������):
;    rdi
;    rsi
;    rdx
;    rcx
;    r8
;    r9

;����������� ������� ������ �� ������ R8 � ������� ������ �� ������ rdx �� 8 ���� � ����� �� rcx ���������� 
;(qnt - rcx, dest - rdx, src - r8)
;��������� Linux: qnt - rdi, dest - rsi, src - rdx
movsdq:
        push r8
		push rcx

        push rdi
		push rsi
		push rdx
		pop  r8
		pop  rdx
		pop  rcx

        cld
		mov rdi, rdx
		mov rsi, r8
		rep movsq

		pop rcx
		pop r8

        ret

;����������� ������� ������ �� ������ R8 � ������� ������ �� ������ rdx �� 4 ����� � ����� �� rcx ����������
movsdd:
        push r8
		push rcx

        push rdi
		push rsi
		push rdx
		pop  r8
		pop  rdx
		pop  rcx

        cld
		mov rdi, rdx
		mov rsi, r8
		rep movsd

		pop rcx
		pop r8

        ret

;����������� ������� ������ �� ������ R8 � ������� ������ �� ������ rdx �� 2 ����� � ����� �� rcx ����������
movsdw:
        push r8
		push rcx

        push rdi
		push rsi
		push rdx
		pop  r8
		pop  rdx
		pop  rcx

        cld
		mov rdi, rdx
		mov rsi, r8
		rep movsw

		pop rcx
		pop r8

        ret

;����������� ������� ������ �� ������ R8 � ������� ������ �� ������ rdx �� 1 ����� � ����� �� rcx ����������
movsdb:
        push r8
		push rcx

        push rdi
		push rsi
		push rdx
		pop  r8
		pop  rdx
		pop  rcx

        cld
		mov rdi, rdx
		mov rsi, r8
		rep movsb

		pop rcx
		pop r8

        ret

;���������� ������� p �� n 8-������� ��������� ������ �� 1 ��������
;���������: rcx - n, rdx - ����� �������
;��������� Linux: rdi - n, rsi - ����� �������
fill8arr:
        push rdi
		push rsi
		push rdx
		push rcx

        push rdi
		push rsi
		pop  rdx
		pop  rcx

        mov rax, [rdx]
		mov rdi, rdx
		cld
		rep stosq

		pop rcx
		pop rdx
		pop rsi
		pop rdi

        ret

;���������� �������� �� ������� �� n 8-������� ��������� (double)
;���������: rcx - n, rdx - ����� ������� ����������, r8 - ����� ��������� �������
nsigmoid:
        finit

		fld1
        fldz
        fsub  qword [r8] 

        ;�������� ��������

        ret

;�������� ������ forward
;���������: rcx - �� 4 ����� n � m, rdx - ����� Z, r8 - ����� ������� X, r9 - ����� ������� Theta
;��������� Linux: rdi - �� 4 ����� n � m, rsi - ����� Z, rdx - ����� ������� X, rcx - ����� ������� Theta
;��������� Linux: rdi - n, rsi - m, rdx - ����� Z, rcx - ����� ������� X, r8 - ����� ������� Theta
forwbs:
        push r10
		push r9

		push rdi
		push rsi
		push rdx
		push rcx
		push r8
		pop  r9                    ;����� Theta
		pop  r8                    ;����� X
		pop  rsi                   ;����� Z
		pop  r10            	   ;m
		pop  rcx                   ;n

        finit

        mov  rdx, r10
l1:		fldz
        mov  rdi, r8
        fld qword [r9] 
		faddp st1, st0
		add  r9, 8

l2:     fld qword [rdi]
        add  rdi, 8
        fmul qword [r9]
		add  r9, 8
		faddp st1, st0
		dec  rdx
		jnz  l2

		mov  rdx, r10
		fstp qword [rsi]
		add  rsi, 8
		dec  rcx
		jnz  l1

		pop r9
		pop r10

        ret

;���������� ��������� Gsum
;���������: rcx - �� 4 ����� n � m, rdx - ����� Gsum, r8 - ����� ������� X, r9 - ����� ������� Delta
;��������� Linux: rdi - �� 4 ����� n � m, rsi - ����� Gsum, rdx - ����� ������� X, rcx - ����� ������� Delta
v8gsum:
        push r10
		push r9
		push r8

		push rdi
		push rsi
		push rdx
		push rcx
		pop  r9
		pop  r8
		pop  rdx
		pop  rcx

        push rdx
		pop  rsi                       ;����� Gsum
		xor  rdx, rdx
		mov  edx, ecx            	   ;m
		shr  rcx, 32                   ;n
		xchg rcx, rdx
		mov  r10, rdx

        finit

l3:		mov  rdi, r8
		mov  rax, [r9]
		mov  [rsi], rax
		add  rsi, 8

l4:     fld qword [rdi]
        add  rdi, 8
        fmul qword [r9]
		fstp qword [rsi]
		add  rsi, 8
		dec  rdx
		jnz  l4

		mov  rdx, r10
		add  r9, 8
		dec  rcx
		jnz  l3

		pop r8
		pop r9
		pop r10

        ret

;��������� ������������ �������� � n 8-�������� ������������ (double)
;���������: rcx - n, rdx - ����� 1 �������, r8 - ����� 2 �������, r9 - ����� ����������, � ������� ����������� ���������
;��������� Linux: rdi - n, rsi - ����� 1 �������, rdx - ����� 2 �������, rcx - ����� ����������, � ������� ����������� ���������
v8mult:
		push r9
		push r8

		push rdi
		push rsi
		push rdx
		push rcx
		pop  r9
		pop  r8
		pop  rdx
		pop  rcx

        finit
		fldz
l5:		or  rcx, rcx
		jz  _ret

        fld  qword [rdx]
        fmul qword [r8]
		faddp st1, st0
		dec rcx
		add rdx, 8
		add r8, 8
		jmp l5

_ret:
        ;fxch st(1)
        fadd qword [r9]
		fstp qword [r9]

		pop r8
		pop r9
		
        ret

;�������� �������� � n 8-�������� ������������ (double), ��������� - � 1 �������
;���������: rcx - n, rdx - ����� 1 �������, r8 - ����� 2 �������
;��������� Linux: rdi - n, rsi - ����� 1 �������, rdx - ����� 2 �������
v8add:
        push  r8
		push  rcx

		push rdi
		push rsi
		push rdx
		pop  r8
		pop  rdx
		pop  rcx

        finit
l6:		
        or  rcx, rcx
		jz  _retv8add

        fld  qword [rdx]
        fadd qword [r8]
		fstp qword [rdx]
		dec rcx
		add rdx, 8
		add r8, 8
		jmp l6

_retv8add:
        pop  rcx
		pop  r8

        ret

;���������������� ������� n * m ����� double
;��������� Windows: rcx - n, rdx - m, r8 - ����� �������� �������, r9 - ����� ����������������� �������
;��������� Linux: rdi - n, rsi - m, rdx - ����� �������� �������, rcx - ����� ����������������� �������
matrix8tran:
        push r15
        push r14
        push r13
		push r12
		push r11
		push r10
        push r9
		push r8
		push rdi
		push rsi
		push rdx
		push rcx
		pop  r9
		pop  r8
		pop  rdx
		pop  rcx

        mov r10, rcx
		shl r10, 3    ;������ ������ � ������ ����������������� �������
		xor r11, r11  ;���������� ����� i
	l7: mov r13, r9
	    mov r14, r11
		shl r14, 3
		add r13, r14
		xor r12, r12  ;���������� ����� j
    l8: mov r15, [r8]
		mov [r13], r15
		add r13, r10
		add r8, 8
		inc r12
		cmp r12, rdx
		jb  l8
		inc r11
		cmp r11, rcx
		jb  l7

		pop rcx
		pop rdx
		pop r8
		pop r9
		pop r10
		pop r11
		pop r12
		pop r13
		pop r14
		pop r15

        ret

