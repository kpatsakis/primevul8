p_b_term(p, cs)
register struct parse *p;
register cset *cs;
{
	register char c;
	register char start, finish;
	register int i;

	/* classify what we've got */
	switch ((MORE()) ? PEEK() : '\0') {
	case '[':
		c = (MORE2()) ? PEEK2() : '\0';
		break;
	case '-':
		SETERROR(REG_ERANGE);
		return;			/* NOTE RETURN */
	default:
		c = '\0';
		break;
	}

	switch (c) {
	case ':':		/* character class */
		NEXT2();
		if(REQUIRE(MORE(), REG_EBRACK)) {}
		c = PEEK();
		if(REQUIRE(c != '-' && c != ']', REG_ECTYPE)) {}
		p_b_cclass(p, cs);
		if(REQUIRE(MORE(), REG_EBRACK)) {}
		if(REQUIRE(EATTWO(':', ']'), REG_ECTYPE)) {}
		break;
	case '=':		/* equivalence class */
		NEXT2();
		if(REQUIRE(MORE(), REG_EBRACK)) {}
		c = PEEK();
		if(REQUIRE(c != '-' && c != ']', REG_ECOLLATE)) {}
		p_b_eclass(p, cs);
		if(REQUIRE(MORE(), REG_EBRACK)) {}
		if(REQUIRE(EATTWO('=', ']'), REG_ECOLLATE)) {}
		break;
	default:		/* symbol, ordinary character, or range */
/* xxx revision needed for multichar stuff */
		start = p_b_symbol(p);
		if (SEE('-') && MORE2() && PEEK2() != ']') {
			/* range */
			NEXT();
			if (EAT('-'))
				finish = '-';
			else
				finish = p_b_symbol(p);
		} else
			finish = start;
/* xxx what about signed chars here... */
		if(REQUIRE(start <= finish, REG_ERANGE)) {}
		for (i = start; i <= finish; i++)
			CHadd(cs, i);
		break;
	}
}