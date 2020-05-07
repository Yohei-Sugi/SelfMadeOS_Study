; hello-os
; TAB=4

        OGR     0x7c00          ; このプログラムがどこに読み込まれるのか

; 以下は標準的なFAT12フォーマットフロッピーディスクのための記述

        JMP     entry
        DB      0x90;
        DB      "HELLOIPL"      ; ブートセクタの名前を自由に記述してよい(ただし8バイトにすること)
        DW      512             ; セクタの大きさ(512バイト固定)
        DB      1               ; クラスタの大きさ(1セクタにしなければいけない)
        DW      1               ; FATがとこから始まるか(通常は1セクタ目からにする)
        DB      2               ; FATの個数(2で固定)
        DW      224             ; ルートディレクトリ領域の大きさ(通常は224エントリとする)
        DW      2880            ; このドライブの大きさ(2880セクタで固定)
        DB      0xf0            ; メディアのタイプ(0xf0で固定)
        DW      9               ; FAT領域の長さ(9セクタのしなければいけない)
        DW      18              ; 1トラックにいくつのセクタがあるか(18で固定)
        DW      2               ; ヘッドの数(2にしなければいけない)
        DD      0               ; パーティションを使っていないのでここは必ず0
        DD      2880            ; このドライブの大きさを再度記述する
        DB      0,0,0x29        ; おまじない
        DD      0xffffffff      ; おそらく墓ユームシリアル番号
        DB      "HELLO-OS   "   ; ディスクの名前(11バイト)
        DB      "FAT12   "      ; フォーマットの名前(8バイト)
        RESB    18              ; とりあえず18バイトあけておく

; プログラム本体

entry:
        MOV     AX,0            ; レジスタの初期化
        MOV     SS,AX
        MOV     SP,0x7c00
        MOV     DS,AX
        MOV     ES,AX

        MOV     SI,msg

putloop:
        MOV     AL,[SI]
        ADD     SI,1            ; SIに1を足す
        CMP     AL,0
        JE      fin
        MOV     AH,0x0e         ; 一文字表示ファンクション
        MOV     BX,15           ; カラーコード
        INT     0x10            ; ビデオBIOS呼び出し
        JMP     putloo

fin:
        HLT                     ; なにかあるまでCPUを停止させる
        JMP     fin             ; 無限ループ

msg:
        DB      0x0a, 0x0a       ; 改行を2つ
        DB      "hello, world"
        DB      0x0a            ; 改行
        DB      0

        RESB    0x7dfe-$        ; 0x7dfeまでを0x00で埋める命令

        DB      0x55, 0xaa
