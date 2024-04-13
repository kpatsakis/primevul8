p_simp_re(p, starordinary)
register struct parse *p;
int starordinary;		/* is a leading * an ordinary character? */
{
	register int c;
	register int count;
	register int count2;
	register sopno pos;
	register int i;
	register sopno subno;
#	define	BACKSL	(1<<CHAR_BIT)

	pos = HERE();		/* repetion op, if any, covers from here */

	assert(MORE());		/* caller should have ensured this */
	c = GETNEXT();
	if (c == '\\') {
		if(REQUIRE(MORE(), REG_EESCAPE)) {}
		c = BACKSL | (unsigned char)GETNEXT();
	}
	switch (c) {
	case '.':
		if (p->g->cflags&REG_NEWLINE)
			nonnewline(p);
		else
			EMIT(OANY, 0);
		break;
	case '[':
		p_bracket(p);
		break;
	case BACKSL|'{':
		SETERROR(REG_BADRPT);
		break;
	case BACKSL|'(':
		p->g->nsub++;
		subno = (sopno) p->g->nsub;
		if (subno < NPAREN)
			p->pbegin[subno] = HERE();
		EMIT(OLPAREN, subno);
		/* the MORE here is an error heuristic */
		if (MORE() && !SEETWO('\\', ')'))
			p_bre(p, '\\', ')');
		if (subno < NPAREN) {
			p->pend[subno] = HERE();
			assert(p->pend[subno] != 0);
		}
		EMIT(ORPAREN, subno);
		if(REQUIRE(EATTWO('\\', ')'), REG_EPAREN)) {}
		break;
	case BACKSL|')':	/* should not get here -- must be user */
	case BACKSL|'}':
		SETERROR(REG_EPAREN);
		break;
	case BACKSL|'1':
	case BACKSL|'2':
	case BACKSL|'3':
	case BACKSL|'4':
	case BACKSL|'5':
	case BACKSL|'6':
	case BACKSL|'7':
	case BACKSL|'8':
	case BACKSL|'9':
		i = (c&~BACKSL) - '0';
		assert(i < NPAREN);
		if (p->pend[i] != 0) {
			assert((uint) i <= p->g->nsub);
			EMIT(OBACK_, i);
			assert(p->pbegin[i] != 0);
			assert(OP(p->strip[p->pbegin[i]]) == OLPAREN);
			assert(OP(p->strip[p->pend[i]]) == ORPAREN);
			(void) dupl(p, p->pbegin[i]+1, p->pend[i]);
			EMIT(O_BACK, i);
		} else
			SETERROR(REG_ESUBREG);
		p->g->backrefs = 1;
		break;
	case '*':
		if(REQUIRE(starordinary, REG_BADRPT)) {}
		/* FALLTHROUGH */
	default:
		ordinary(p, c &~ BACKSL);
		break;
	}

	if (EAT('*')) {		/* implemented as +? */
		/* this case does not require the (y|) trick, noKLUDGE */
		INSERT(OPLUS_, pos);
		ASTERN(O_PLUS, pos);
		INSERT(OQUEST_, pos);
		ASTERN(O_QUEST, pos);
	} else if (EATTWO('\\', '{')) {
		count = p_count(p);
		if (EAT(',')) {
			if (MORE() && my_isdigit(p->charset,PEEK())) {
				count2 = p_count(p);
				if(REQUIRE(count <= count2, REG_BADBR)) {}
			} else		/* single number with comma */
				count2 = RE_INFINITY;
		} else		/* just a single number */
			count2 = count;
		repeat(p, pos, count, count2);
		if (!EATTWO('\\', '}')) {	/* error heuristics */
			while (MORE() && !SEETWO('\\', '}'))
				NEXT();
			if(REQUIRE(MORE(), REG_EBRACE)) {}
			SETERROR(REG_BADBR);
		}
	} else if (c == (unsigned char)'$')	/* $ (but not \$) ends it */
		return(1);

	return(0);
}