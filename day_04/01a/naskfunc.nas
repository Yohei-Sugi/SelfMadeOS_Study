; naskfunc
; TAB=4

[FORMAT "WCOFF"]			; オブジェクトファイルを作るモード
[INSTRSET "i486p"]			; 486の命令まで使いたいという記述
[BITS 32]					; 32ビットモード用の機械語を生成する


; オブジェクトファイルのための情報
[FILE "naskfunc.nas"]		; ソースファイル名情報

		GLOBAL	_io_hlt,_write_mem8	; このプログラムに含まれる関数名


; 以下は実際の関数

[SECTION .text]		; オブジェクトファイルではこれを書いてからプログラムを書く

_io_hlt:	; void io_hlt(void);
		HLT
		RET

_write_mem8:	; void write_mem8(int addr, int data);
		MOV		ECX,[ESP+4]		; [ESP+4]にaddrが入っているので、それをECXに読み込む
		MOV		AL,[ESP+8]		; [ESP+8]にdataが入っているので、それをALに読み込む
		MOV		[ECX],AL
		RET
