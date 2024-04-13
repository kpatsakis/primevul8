p_count(p)
register struct parse *p;
{
	register int count = 0;
	register int ndigits = 0;

	while (MORE() && my_isdigit(p->charset,PEEK()) && count <= DUPMAX) {
		count = count*10 + (GETNEXT() - '0');
		ndigits++;
	}

	if(REQUIRE(ndigits > 0 && count <= DUPMAX, REG_BADBR)) {}
	return(count);
}