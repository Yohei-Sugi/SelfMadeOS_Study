/* 他のファイルで作った関数があります!とC言語コンパイラに教える。宣言やね */
void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen(char *vram, int x, int y);
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);

#define COL8_000000		 0	/* 黒 */
#define COL8_FF0000		 1	/* 明るい赤 */
#define COL8_00FF00		 2	/* 明るい緑 */
#define COL8_FFFF00		 3	/* 明るい黄色 */
#define COL8_0000FF		 4	/* 明るい青 */
#define COL8_FF00FF		 5	/* 明るい紫 */
#define COL8_00FFFF		 6	/* 明るい水色 */
#define COL8_FFFFFF		 7	/* 白 */
#define COL8_C6C6C6		 8	/* 明るい灰色 */
#define COL8_840000		 9	/* 暗い赤 */
#define COL8_008400		10	/* 暗い緑 */
#define COL8_848400		11	/* 暗い黄色 */
#define COL8_000084		12	/* 暗い青 */
#define COL8_840084		13	/* 暗い紫 */
#define COL8_008484		14	/* 暗い水色 */
#define COL8_848484		15	/* 暗い灰色 */

/* 起動時の情報を持つstruct。アドレス直接指定するので、順番は変えられないstruct。 */
struct BOOTINFO
{
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;					/* 画面のサイズ用変数 */
	char *vram;							/* 画面情報の番地を格納する */
};

void HariMain(void)
{

	/* 画面モードなどの値を、asmhead.nasから読み取るためのstructポインタ */
	struct BOOTINFO *binfo = (struct BOOTINFO *)0x0ff0;

	/* パレットを設定 */
	init_palette();

	/* 背景画面を描く */
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);

	/* 文字列をいくつか並べてみる */
	putfonts8_asc(binfo->vram, binfo->scrnx, 31, 31, COL8_000000, "YOU GOT BOOST POWER !!");
	putfonts8_asc(binfo->vram, binfo->scrnx, 29, 29, COL8_000000, "YOU GOT BOOST POWER !!");
	putfonts8_asc(binfo->vram, binfo->scrnx, 31, 29, COL8_000000, "YOU GOT BOOST POWER !!");
	putfonts8_asc(binfo->vram, binfo->scrnx, 29, 31, COL8_000000, "YOU GOT BOOST POWER !!");
	putfonts8_asc(binfo->vram, binfo->scrnx, 30, 31, COL8_000000, "YOU GOT BOOST POWER !!");
	putfonts8_asc(binfo->vram, binfo->scrnx, 30, 29, COL8_000000, "YOU GOT BOOST POWER !!");
	putfonts8_asc(binfo->vram, binfo->scrnx, 31, 30, COL8_000000, "YOU GOT BOOST POWER !!");
	putfonts8_asc(binfo->vram, binfo->scrnx, 29, 30, COL8_000000, "YOU GOT BOOST POWER !!");
	putfonts8_asc(binfo->vram, binfo->scrnx, 30, 30, COL8_FF0000, "YOU GOT BOOST POWER !!");

	/* 無限ループ */
	for ( ; ; )
	{
		io_hlt();
	}
}

void init_palette(void)
{
	/* カラーパレットの作成。16色あればなんとかなる */
	/* static char 命令は、データにしか使えないけど、DB命令相当 */
	static unsigned char table_rgb[16 * 3] = {
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

	for ( i = start; i <= end; ++i )
	{
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	/* 割り込み許可フラグをもとに戻す */
	return;
}

void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1)
{
	/* x0, y0, x1, y1 は範囲チェックしたいな。精神的にも。それいうと、set_paletteもか。。 */

	int x, y;
	for ( y = y0; y <= y1; ++y )
	{
		for ( x = x0; x <= x1; ++x )
		{
			vram[y * xsize + x] = c;
		}
	}
	return;
}

void init_screen(char *vram, int x, int y)
{
	/* 頑張って絵を描く */
	boxfill8(vram, x, COL8_008484,      0,      0, x -  1, y - 29);
	boxfill8(vram, x, COL8_C6C6C6,      0, y - 28, x -  1, y - 28);
	boxfill8(vram, x, COL8_FFFFFF,      0, y - 27, x -  1, y - 27);
	boxfill8(vram, x, COL8_C6C6C6,      0, y - 26, x -  1, y -  1);

	boxfill8(vram, x, COL8_FFFFFF,      3, y - 24,     59, y - 24);
	boxfill8(vram, x, COL8_FFFFFF,      2, y - 24,      2, y -  4);
	boxfill8(vram, x, COL8_848484,      3, y -  4,     59, y -  4);
	boxfill8(vram, x, COL8_848484,     59, y - 23,     59, y -  5);
	boxfill8(vram, x, COL8_000000,      2, y -  3,     59, y -  3);
	boxfill8(vram, x, COL8_000000,     60, y - 24,     60, y -  3);

	boxfill8(vram, x, COL8_848484, x - 47, y - 24, x -  4, y - 24);
	boxfill8(vram, x, COL8_848484, x - 47, y - 23, x - 47, y -  4);
	boxfill8(vram, x, COL8_FFFFFF, x - 47, y -  3, x -  4, y -  3);
	boxfill8(vram, x, COL8_FFFFFF, x -  3, y - 24, x -  3, y -  3);

	return;
}

void putfont8(char *vram, int xsize, int x, int y, char c, char *font)
{
	int i;
	char *p, data;

	for ( i = 0; i < 16; ++i )
	{
		p = vram + (y + i) * xsize + x;
		data = font[i];
		if ((data & 0x80) != 0) { p[0] = c; }
		if ((data & 0x40) != 0) { p[1] = c; }
		if ((data & 0x20) != 0) { p[2] = c; }
		if ((data & 0x10) != 0) { p[3] = c; }
		if ((data & 0x08) != 0) { p[4] = c; }
		if ((data & 0x04) != 0) { p[5] = c; }
		if ((data & 0x02) != 0) { p[6] = c; }
		if ((data & 0x01) != 0) { p[7] = c; }
	}
	
	return;
}

void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s)
{
	/* hankaku.txt -> (makefont.exeでhankaku.bin) -> (bin2obj.exeでhankaku.obj)
	   -> 情報のリンクができるようになるので、C側でexternを使うことで呼び出し可能になる  */
	extern char hankaku[4096];

	for ( ; *s != 0x00; ++s )	/* ここでnull文字チェック+次の文字へ　という動き */
	{
		putfont8(vram, xsize, x, y, c, hankaku + (*s) * 16);
		x += 8;	/* 描画先のx座標をずらしている */
	}
	return;
}