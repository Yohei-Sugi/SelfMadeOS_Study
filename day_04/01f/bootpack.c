/* 他のファイルで作った関数があります!とC言語コンパイラに教える。宣言やね */
void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);

/* 関数宣言なのに、{}がなくて、いきなり;を書くと、
　　他のファイルにあるからよろしくね、という意味になる。うん、宣言やね */
/* void write_mem8(int addr, int data); */

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);

void HariMain(void)
{
	/* 変数宣言。 iという変数は、32ビットの整数型 */
	int i;

	/* pという変数は、BYTE [...]用の番地 */
	char *p;

	/* パレットを設定 */
	init_palette();

	p = (char *)0xa0000; /* 番地を代入 */

	for ( i = 0xa0000; i <= 0xaffff; ++i )
	{
		p[i] = i & 0x0f; /* でてきた。 C言語の闇と言われるけど、アセンブラの発展型と捉えるとそうでもないよねー。 */
		/* *(p + i) = i & 0x0f; */
		/* これで、下記と同じ動作をする */
		/* アドレスをずらしているだけやもんね。 *(p+i) と p[i] は一緒やし */

		/* p = i;* / /* 番地を代入 */
		/* *p = (char *)i & 0x0f; */
		/* これで、下記処理と同等の動きをする */

		/* 16ピクセルごとに異なる色番号を繰り返す */
		/* write_mem8(i, i & 0x0f); */
	}

	for ( ; ; )
	{
		io_hlt();
	}
}

void init_palette(void)
{
	/* カラーパレットの作成。16色あればなんとかなる */
	/* static char 命令は、データにしか使えないけど、DB命令相当 */
	static unsigned char table_rgb[16 * 3] =
	{
		0x00, 0x00, 0x00,	/*  0:黒 */
		0xff, 0x00, 0x00,	/*  1:明るい赤 */
		0x00, 0xff, 0x00,	/*  2:明るい緑 */
		0xff, 0xff, 0x00,	/*  3:明るい黄色 */
		0x00, 0x00, 0xff,	/*  4:明るい青 */
		0xff, 0x00, 0xff,	/*  5:明るい紫 */
		0x00, 0xff, 0xff,	/*  6:明るい水色 */
		0xff, 0xff, 0xff,	/*  7:白 */
		0xc6, 0xc6, 0xc6,	/*  8:明るい灰色 */
		0x84, 0x00, 0x00,	/*  9:暗い赤 */
		0x00, 0x84, 0x00,	/* 10:暗い緑 */
		0x84, 0x84, 0x00,	/* 11:暗い黄色 */
		0x00, 0x00, 0x84,	/* 12:暗い青 */
		0x84, 0x00, 0x84,	/* 13:暗い紫 */
		0x00, 0x84, 0x84,	/* 14:暗い水色 */
		0x84, 0x84, 0x84,	/* 15:暗い灰色 */
	};
	set_palette(0, 15, table_rgb);
	return;
}

void set_palette(int start, int end, unsigned char *rgb)
{
	int i, eflags;

	eflags = io_load_eflags();	/* 割り込み許可フラグの値を記録する */
	io_cli();					/* 許可フラグを0にして割り込み禁止にする */
	io_out8(0x03c8, start);

	for ( i = start, i <= end; ++i )
	{
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	/* 割り込み許可フラグをもとに戻す */
	return;
}
