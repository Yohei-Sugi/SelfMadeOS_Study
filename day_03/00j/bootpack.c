/* 他のファイルで作った関数があります!とC言語コンパイラに教える。宣言やね */
void io_hlt(void);

/* 関数宣言なのに、{}がなくて、いきなり;を書くと、
　　他のファイルにあるからよろしくね、という意味になる。うん、宣言やね */

void HariMain(void)
{

fin:
	io_hlt();	/* これでnaskfunc.nasの_io_hltが実行される */
	goto fin;

}
