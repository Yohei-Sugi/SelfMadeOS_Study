/* 他のファイルで作った関数があります!とC言語コンパイラに教える。宣言やね */
void io_hlt(void);

/* 関数宣言なのに、{}がなくて、いきなり;を書くと、
　　他のファイルにあるからよろしくね、という意味になる。うん、宣言やね */
void write_mem8(int addr, int data);

void HariMain(void)
{
	/* 変数宣言。 iという変数は、32ビットの整数型 */
	int i;

	for ( i = 0xa0000; i <= 0xaffff; ++i )
	{
		write_mem8(i, 15);	/* MOV BYTE [i],15 ; としたい */
	}

	for ( ; ; )
	{
		io_hlt();
	}
}
