#include <stdio.h>	/* GO(ジーオー)というコンパイラを使っており、sprintfを使うためinclude。*/

/* 他のファイルで作った関数があります!とC言語コンパイラに教える。宣言やね */
void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen8(char *vram, int x, int y);
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);
void init_mouse_cursor8(char *mouse, char backgroundColor);
void putblock8_8(char *vram, int vxsize, int pxsize, int pysize, int px0, int py0, char *buf, int bxsize);

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

/* GDTの8バイトの中身で、CPUの資料を元に構造体として書いたもの */
struct SEGMENT_DESCRIPTOR
{
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};

/* IDTの8倍との中身で、CPUの資料を元に構造体として書いたもの */
struct GATE_DESCRIPTOR
{
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
};

void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);

void HariMain(void)
{

	/* 画面モードなどの値を、asmhead.nasから読み取るためのstructポインタ */
	struct BOOTINFO *binfo = (struct BOOTINFO *)0x0ff0;

	/* 画面に表示するためのメモリを用意 */
	char s[40], mouseCursor[256];

	/* マウスの表示座標を格納する */
	int mouseX, mouseY;

	/* GDT-IDTの初期化 */
	init_gdtidt();

	/* パレットを設定 */
	init_palette();

	/* 背景画面を描く */
	init_screen8(binfo->vram, binfo->scrnx, binfo->scrny);

	/* マウスカーソルが画面中央になるように座標計算 */
	mouseX = (binfo->scrnx - 16) / 2;
	mouseY = (binfo->scrny - 28 - 16) / 2;

	/* マウスカーソルを表示 */
	init_mouse_cursor8(mouseCursor, COL8_008484);
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mouseX, mouseY, mouseCursor, 16);

	/* マウスカーソルの表示位置を画面に表示する */
	sprintf(s, "(x, y) = (%d, %d)", mouseX, mouseY);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);

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

void init_screen8(char *vram, int x, int y)
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

/* マウスカーソルを準備 */
void init_mouse_cursor8(char *mouse, char backgroundColor)
{
	static char cursor[16][16] = {
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
		"*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
		"**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
		".............***"
	};
	int x, y;

	for ( y = 0; y < 16; ++y )
	{
		for ( x = 0; x < 16; ++x )
		{
			if ( cursor[y][x] == '*' )
			{
				mouse[y * 16 + x] = COL8_000000;
			}
			else if ( cursor[y][x] == 'O' )
			{
				mouse[y * 16 + x] = COL8_FFFFFF;
			}
			else if ( cursor[y][x] == '.' )
			{
				mouse[y * 16 + x] = backgroundColor;
			}
		}
	}
	return;
}

void putblock8_8(char *vram, int vxsize, int pxsize, int pysize, int px0, int py0, char *buf, int bxsize)
{
	int x, y;
	for ( y = 0; y < pysize; ++y )
	{
		for ( x = 0; x < pxsize; x++ )
		{
			vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
		}
	}
	return;
}

/* GDTとIDTの初期化 */
void init_gdtidt(void)
{
	struct SEGMENT_DESCRIPTOR 	*gdt = (struct SEGMENT_DESCRIPTOR *) 0x0027000;
	struct GATE_DESCRIPTOR		*idt = (struct GATE_DESCRIPTOR 	  *) 0x26f8000;
	int i;

	/* GDTの初期化 */
	for ( i = 0; i < 8192; ++i )
	{
		set_segmdesc(gdt + i, 0, 0, 0);
	}
	set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, 0x4092);
	set_segmdesc(gdt + 2, 0x0007ffff, 0x00280000, 0x409a);
	load_gdtr(0xffff, 0x00270000);

	/* IDTの初期化 */
	for ( i = 0; i < 256; ++i )
	{
		set_gatedesc(idt + i, 0, 0, 0);
	}
	load_idtr(0x7ff, 0x0026f800);

	return;
}

void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar)
{
	if ( limit > 0xfffff )
	{
		ar |= 0x8000;	/* G_bit = 1 */
		limit /= 0x1000;
	}
	sd->limit_low		= limit & 0xffff;
	sd->base_low		= base  & 0xffff;
	sd->base_mid		= (base >> 16) & 0xff;
	sd->access_right	= ar & 0xff;
	sd->limit_high		= ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high		= (base >> 24) & 0xff;
	return;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar)
{
	gd->offset_low		= offset & 0xffff;
	gd->selector		= selector;
	gd->dw_count		= (ar >> 8) & 0xff;
	gd->access_right	= ar & 0xff;
	gd->offset_high		= (offset >> 16) & 0xffff;
	return;
}
