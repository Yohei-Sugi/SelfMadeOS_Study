; hello-os
; TAB=4

CYLS	EQU		10				; どこまで読み込むか

		OGR		0x7c00			; このプログラムがどこに読み込まれるのか

; 以下は標準的なFAT12フォーマットフロッピーディスクのための記述

		JMP		entry
		DB		0x90;
		DB		"HELLOIPL"		; ブートセクタの名前を自由に記述してよい(ただし8バイトにすること)
		DW		512				; セクタの大きさ(512バイト固定)
		DB		1				; クラスタの大きさ(1セクタにしなければいけない)
		DW		1				; FATがとこから始まるか(通常は1セクタ目からにする)
		DB		2				; FATの個数(2で固定)
		DW		224				; ルートディレクトリ領域の大きさ(通常は224エントリとする)
		DW		2880			; このドライブの大きさ(2880セクタで固定)
		DB		0xf0			; メディアのタイプ(0xf0で固定)
		DW		9				; FAT領域の長さ(9セクタのしなければいけない)
		DW		18				; 1トラックにいくつのセクタがあるか(18で固定)
		DW		2				; ヘッドの数(2にしなければいけない)
		DD		0				; パーティションを使っていないのでここは必ず0
		DD		2880			; このドライブの大きさを再度記述する
		DB		0,0,0x29		; おまじない
		DD		0xffffffff		; おそらく墓ユームシリアル番号
		DB		"HELLO-OS   "	; ディスクの名前(11バイト)
		DB		"FAT12   "		; フォーマットの名前(8バイト)
		RESB	18				; とりあえず18バイトあけておく

; プログラム本体

entry:
		MOV		AX,0			; レジスタの初期化
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX

; ディスクを読む

		MOV		AX,0x0820
		MOV		ES,AX
		MOV		CH,0			; シリンダ0
		MOV		DH,0			; ヘッド0
		MOV		CL,2			; セクタ2
readloop:

		MOV		SI,0			; 失敗回数を数えるレジスタ
retry:
		MOV		AH,0x02			; AH=0x02 : ディスク読み込み
		MOV		AL,1			; 1セクタ
		MOV		BX,0
		MOV		DL,0x00			; Aドライブ
		INT		0x13			; ディスクBIOS呼び出し
		JNC		next			; エラーが起きなければnextへ
		ADD		SI,1			; SIに1を足す
		CMP		SI,5			; SIと5を比較
		JAE		error			; SI >= 5 だったらerrorへ
		MOV		AH,0x00
		MOV		DL,0x00			; Aドライブ
		INT		0x13			; ドライブのリセット
		JC		retry

next:
		MOV		AX,ES			; アドレスを0x200進める
		ADD		AX,0x0020
		MOV		EX,AX			; ADD EX,0x020 という命令がないため一度、AXに移す必要がある
		ADD		CL,1			; CLに1を足す
		ADD		CL,18			; CLと18を比較
		JBE		readloop		; CL <= 18 だったらreadloopへ
		MOV		CL,1
		ADD		DH,1
		CMP		DH,2
		JB		readloop		; DH < 2 だったらreadloopへ
		MOV		DH,0
		ADD		CH,1
		CMP		CH,CYLS
		JB		readloop		; CH < CYLS だったらreadloopへ

; 読み終わったので、haribote.sysを実行
		MOV		[0x0ff0],CH		; IPLがどこまで読んだのかをメモ
		JMP		0xc200

error:
		MOV		SI,msg

putloop:
		MOV		AL,[SI]
		ADD		SI,1			; SIに1を足す
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; 一文字表示ファンクション
		MOV		BX,15			; カラーコード
		INT		0x10			; ビデオBIOS呼び出し
		JMP		putloo

msg:
		DB		0x0a, 0x0a		; 改行を2つ
		DB		"hello, world"
		DB		0x0a			; 改行
		DB		0

		RESB	0x7dfe-$		; 0x7dfeまでを0x00で埋める命令

		DB		0x55, 0xaa
