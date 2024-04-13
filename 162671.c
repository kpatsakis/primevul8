p_ere(p, stop)
register struct parse *p;
int stop;			/* character this ERE should end at */
{
	register char c;
	register sopno UNINIT_VAR(prevback);
	register sopno UNINIT_VAR(prevfwd);
	register sopno conc;
	register int first = 1;		/* is this the first alternative? */

	for (;;) {
		/* do a bunch of concatenated expressions */
		conc = HERE();
		while (MORE() && (c = PEEK()) != '|' && c != stop)
		{
		  if (my_regex_enough_mem_in_stack &&
		      my_regex_enough_mem_in_stack(0))
		  {
		    SETERROR(REG_ESPACE);
		    return;
		  }
		  p_ere_exp(p);
		}
		if(REQUIRE(HERE() != conc, REG_EMPTY)) {}/* require nonempty */

		if (!EAT('|'))
			break;		/* NOTE BREAK OUT */

		if (first) {
			INSERT(OCH_, conc);	/* offset is wrong */
			prevfwd = conc;
			prevback = conc;
			first = 0;
		}
		ASTERN(OOR1, prevback);
		prevback = THERE();
		AHEAD(prevfwd);			/* fix previous offset */
		prevfwd = HERE();
		EMIT(OOR2, 0);			/* offset is very wrong */
	}

	if (!first) {		/* tail-end fixups */
		AHEAD(prevfwd);
		ASTERN(O_CH, prevback);
	}

	assert(!MORE() || SEE(stop));
}