#include <stdio.h>	/* GO(�W�[�I�[)�Ƃ����R���p�C�����g���Ă���Asprintf���g������include�B*/

/* ���̃t�@�C���ō�����֐�������܂�!��C����R���p�C���ɋ�����B�錾��� */
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

#define COL8_000000		 0	/* �� */
#define COL8_FF0000		 1	/* ���邢�� */
#define COL8_00FF00		 2	/* ���邢�� */
#define COL8_FFFF00		 3	/* ���邢���F */
#define COL8_0000FF		 4	/* ���邢�� */
#define COL8_FF00FF		 5	/* ���邢�� */
#define COL8_00FFFF		 6	/* ���邢���F */
#define COL8_FFFFFF		 7	/* �� */
#define COL8_C6C6C6		 8	/* ���邢�D�F */
#define COL8_840000		 9	/* �Â��� */
#define COL8_008400		10	/* �Â��� */
#define COL8_848400		11	/* �Â����F */
#define COL8_000084		12	/* �Â��� */
#define COL8_840084		13	/* �Â��� */
#define COL8_008484		14	/* �Â����F */
#define COL8_848484		15	/* �Â��D�F */

/* �N�����̏�������struct�B�A�h���X���ڎw�肷��̂ŁA���Ԃ͕ς����Ȃ�struct�B */
struct BOOTINFO
{
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;					/* ��ʂ̃T�C�Y�p�ϐ� */
	char *vram;							/* ��ʏ��̔Ԓn���i�[���� */
};

/* GDT��8�o�C�g�̒��g�ŁACPU�̎��������ɍ\���̂Ƃ��ď��������� */
struct SEGMENT_DESCRIPTOR
{
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};

/* IDT��8�{�Ƃ̒��g�ŁACPU�̎��������ɍ\���̂Ƃ��ď��������� */
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

	/* ��ʃ��[�h�Ȃǂ̒l���Aasmhead.nas����ǂݎ�邽�߂�struct�|�C���^ */
	struct BOOTINFO *binfo = (struct BOOTINFO *)0x0ff0;

	/* ��ʂɕ\�����邽�߂̃�������p�� */
	char s[40], mouseCursor[256];

	/* �}�E�X�̕\�����W���i�[���� */
	int mouseX, mouseY;

	/* GDT-IDT�̏����� */
	init_gdtidt();

	/* �p���b�g��ݒ� */
	init_palette();

	/* �w�i��ʂ�`�� */
	init_screen8(binfo->vram, binfo->scrnx, binfo->scrny);

	/* �}�E�X�J�[�\������ʒ����ɂȂ�悤�ɍ��W�v�Z */
	mouseX = (binfo->scrnx - 16) / 2;
	mouseY = (binfo->scrny - 28 - 16) / 2;

	/* �}�E�X�J�[�\����\�� */
	init_mouse_cursor8(mouseCursor, COL8_008484);
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mouseX, mouseY, mouseCursor, 16);

	/* �}�E�X�J�[�\���̕\���ʒu����ʂɕ\������ */
	sprintf(s, "(x, y) = (%d, %d)", mouseX, mouseY);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);

	/* �������[�v */
	for ( ; ; )
	{
		io_hlt();
	}
}

void init_palette(void)
{
	/* �J���[�p���b�g�̍쐬�B16�F����΂Ȃ�Ƃ��Ȃ� */
	/* static char ���߂́A�f�[�^�ɂ����g���Ȃ����ǁADB���ߑ��� */
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*  0:�� */
		0xff, 0x00, 0x00,	/*  1:���邢�� */
		0x00, 0xff, 0x00,	/*  2:���邢�� */
		0xff, 0xff, 0x00,	/*  3:���邢���F */
		0x00, 0x00, 0xff,	/*  4:���邢�� */
		0xff, 0x00, 0xff,	/*  5:���邢�� */
		0x00, 0xff, 0xff,	/*  6:���邢���F */
		0xff, 0xff, 0xff,	/*  7:�� */
		0xc6, 0xc6, 0xc6,	/*  8:���邢�D�F */
		0x84, 0x00, 0x00,	/*  9:�Â��� */
		0x00, 0x84, 0x00,	/* 10:�Â��� */
		0x84, 0x84, 0x00,	/* 11:�Â����F */
		0x00, 0x00, 0x84,	/* 12:�Â��� */
		0x84, 0x00, 0x84,	/* 13:�Â��� */
		0x00, 0x84, 0x84,	/* 14:�Â����F */
		0x84, 0x84, 0x84,	/* 15:�Â��D�F */
	};
	set_palette(0, 15, table_rgb);
	return;
}

void set_palette(int start, int end, unsigned char *rgb)
{
	int i, eflags;

	eflags = io_load_eflags();	/* ���荞�݋��t���O�̒l���L�^���� */
	io_cli();					/* ���t���O��0�ɂ��Ċ��荞�݋֎~�ɂ��� */
	io_out8(0x03c8, start);

	for ( i = start; i <= end; ++i )
	{
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	/* ���荞�݋��t���O�����Ƃɖ߂� */
	return;
}

void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1)
{
	/* x0, y0, x1, y1 �͔͈̓`�F�b�N�������ȁB���_�I�ɂ��B���ꂢ���ƁAset_palette�����B�B */

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
	/* �撣���ĊG��`�� */
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
	/* hankaku.txt -> (makefont.exe��hankaku.bin) -> (bin2obj.exe��hankaku.obj)
	   -> ���̃����N���ł���悤�ɂȂ�̂ŁAC����extern���g�����ƂŌĂяo���\�ɂȂ�  */
	extern char hankaku[4096];

	for ( ; *s != 0x00; ++s )	/* ������null�����`�F�b�N+���̕����ց@�Ƃ������� */
	{
		putfont8(vram, xsize, x, y, c, hankaku + (*s) * 16);
		x += 8;	/* �`����x���W�����炵�Ă��� */
	}
	return;
}

/* �}�E�X�J�[�\�������� */
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

/* GDT��IDT�̏����� */
void init_gdtidt(void)
{
	struct SEGMENT_DESCRIPTOR 	*gdt = (struct SEGMENT_DESCRIPTOR *) 0x0027000;
	struct GATE_DESCRIPTOR		*idt = (struct GATE_DESCRIPTOR 	  *) 0x26f8000;
	int i;

	/* GDT�̏����� */
	for ( i = 0; i < 8192; ++i )
	{
		set_segmdesc(gdt + i, 0, 0, 0);
	}
	set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, 0x4092);
	set_segmdesc(gdt + 2, 0x0007ffff, 0x00280000, 0x409a);
	load_gdtr(0xffff, 0x00270000);

	/* IDT�̏����� */
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
