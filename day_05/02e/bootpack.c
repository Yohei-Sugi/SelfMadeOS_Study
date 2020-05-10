/* ���̃t�@�C���ō�����֐�������܂�!��C����R���p�C���ɋ�����B�錾��� */
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

void HariMain(void)
{

	/* ��ʃ��[�h�Ȃǂ̒l���Aasmhead.nas����ǂݎ�邽�߂�struct�|�C���^ */
	struct BOOTINFO *binfo = (struct BOOTINFO *)0x0ff0;

	/* hankaku.txt -> (makefont.exe��hankaku.bin) -> (bin2obj.exe��hankaku.obj)
	   -> ���̃����N���ł���悤�ɂȂ�̂ŁAC����extern���g�����ƂŌĂяo���\�ɂȂ�  */
	extern char hankaku[4096];

	/* �p���b�g��ݒ� */
	init_palette();

	/* �w�i��ʂ�`�� */
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);

	/* ���������������ׂĂ݂� */
	putfont8(binfo->vram, binfo->scrnx,  8, 8, COL8_FFFFFF, hankaku + 'A' * 16);
	putfont8(binfo->vram, binfo->scrnx, 16, 8, COL8_FFFFFF, hankaku + 'B' * 16);
	putfont8(binfo->vram, binfo->scrnx, 24, 8, COL8_FFFFFF, hankaku + 'C' * 16);
	putfont8(binfo->vram, binfo->scrnx, 32, 8, COL8_FFFFFF, hankaku + '1' * 16);
	putfont8(binfo->vram, binfo->scrnx, 40, 8, COL8_FFFFFF, hankaku + '2' * 16);
	putfont8(binfo->vram, binfo->scrnx, 48, 8, COL8_FFFFFF, hankaku + '3' * 16);
	/* hankaku.txt��ASCII�R�[�h���ɕ���ł���̂ŁA�R�[�h��char�^�𑫂��B16���|���Ă���̂́A�����f�[�^�T�C�Y����� */

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

void init_screen(char *vram, int x, int y)
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