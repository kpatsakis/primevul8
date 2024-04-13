p_str(p)
register struct parse *p;
{
	if(REQUIRE(MORE(), REG_EMPTY)) {}
	while (MORE())
		ordinary(p, GETNEXT());
}