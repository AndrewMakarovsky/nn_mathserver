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

;В Linux применяемый интерфейс ABI еще называется "System V ABI".
;Согласно условностям первые 6 параметров передаются в функцию через следующие регистры (в порядке следования параметров):
;    rdi
;    rsi
;    rdx
;    rcx
;    r8
;    r9

;Копирование области памяти по адресу R8 в область памяти по адресу rdx по 8 байт в цикле из rcx повторений 
;(qnt - rcx, dest - rdx, src - r8)
;Параметры Linux: qnt - rdi, dest - rsi, src - rdx
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

;Копирование области памяти по адресу R8 в область памяти по адресу rdx по 4 байта в цикле из rcx повторений
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

;Копирование области памяти по адресу R8 в область памяти по адресу rdx по 2 байта в цикле из rcx повторений
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

;Копирование области памяти по адресу R8 в область памяти по адресу rdx по 1 байту в цикле из rcx повторений
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

;Заполнение массива p из n 8-байтных элементов числом из 1 элемента
;Параметры: rcx - n, rdx - адрес массива
;Параметры Linux: rdi - n, rsi - адрес массива
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

;Вычисление сигмоиды от вектора из n 8-байтных элементов (double)
;Параметры: rcx - n, rdx - адрес вектора результата, r8 - адрес исходного вектора
nsigmoid:
        finit

		fld1
        fldz
        fsub  qword [r8] 

        ;Дописать алгоритм

        ret

;Основной расчет forward
;Параметры: rcx - по 4 байта n и m, rdx - адрес Z, r8 - адрес вектора X, r9 - адрес матрицы Theta
;Параметры Linux: rdi - по 4 байта n и m, rsi - адрес Z, rdx - адрес вектора X, rcx - адрес матрицы Theta
;Параметры Linux: rdi - n, rsi - m, rdx - адрес Z, rcx - адрес вектора X, r8 - адрес матрицы Theta
forwbs:
        push r10
		push r9

		push rdi
		push rsi
		push rdx
		push rcx
		push r8
		pop  r9                    ;адрес Theta
		pop  r8                    ;адрес X
		pop  rsi                   ;адрес Z
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

;Вычисление градиента Gsum
;Параметры: rcx - по 4 байта n и m, rdx - адрес Gsum, r8 - адрес вектора X, r9 - адрес вектора Delta
;Параметры Linux: rdi - по 4 байта n и m, rsi - адрес Gsum, rdx - адрес вектора X, rcx - адрес вектора Delta
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
		pop  rsi                       ;адрес Gsum
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

;Скалярное произведение векторов с n 8-байтными компонентами (double)
;Параметры: rcx - n, rdx - адрес 1 вектора, r8 - адрес 2 вектора, r9 - адрес переменной, к которой добавляется результат
;Параметры Linux: rdi - n, rsi - адрес 1 вектора, rdx - адрес 2 вектора, rcx - адрес переменной, к которой добавляется результат
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

;Сложение векторов с n 8-байтными компонентами (double), результат - в 1 векторе
;Параметры: rcx - n, rdx - адрес 1 вектора, r8 - адрес 2 вектора
;Параметры Linux: rdi - n, rsi - адрес 1 вектора, rdx - адрес 2 вектора
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

;Транспонирование матрицы n * m чисел double
;Параметры Windows: rcx - n, rdx - m, r8 - адрес исходной матрицы, r9 - адрес транспонированной матрицы
;Параметры Linux: rdi - n, rsi - m, rdx - адрес исходной матрицы, rcx - адрес транспонированной матрицы
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
		shl r10, 3    ;Размер строки в байтах транспонированной матрицы
		xor r11, r11  ;Переменная цикла i
	l7: mov r13, r9
	    mov r14, r11
		shl r14, 3
		add r13, r14
		xor r12, r12  ;Переменная цикла j
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

