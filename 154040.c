print_sep(struct table *t, int row, int type, int maxcol, Str buf)
{
    int forbid;
    int rule_mode;
    int i, k, l, m;

    if (row >= 0)
	check_row(t, row);
    check_row(t, row + 1);
    if ((type == T_TOP || type == T_BOTTOM) && t->border_mode == BORDER_THICK) {
	rule_mode = BORDER_THICK;
    }
    else {
	rule_mode = BORDER_THIN;
    }
    forbid = 1;
    if (type == T_TOP)
	forbid |= 2;
    else if (type == T_BOTTOM)
	forbid |= 8;
    else if (t->tabattr[row + 1][0] & HTT_Y) {
	forbid |= 4;
    }
    if (t->border_mode != BORDER_NOWIN) {
	push_symbol(buf, RULE(t->border_mode, forbid), symbol_width, 1);
    }
    for (i = 0; i <= maxcol; i++) {
	forbid = 10;
	if (type != T_BOTTOM && (t->tabattr[row + 1][i] & HTT_Y)) {
	    if (t->tabattr[row + 1][i] & HTT_X) {
		goto do_last_sep;
	    }
	    else {
		for (k = row;
		     k >= 0 && t->tabattr[k] && (t->tabattr[k][i] & HTT_Y);
		     k--) ;
		m = t->tabwidth[i] + 2 * t->cellpadding;
		for (l = i + 1; l <= t->maxcol && (t->tabattr[row][l] & HTT_X);
		     l++)
		    m += t->tabwidth[l] + t->cellspacing;
		print_item(t, k, i, m, buf);
	    }
	}
	else {
	    int w = t->tabwidth[i] + 2 * t->cellpadding;
	    if (RULE_WIDTH == 2)
		w = (w + 1) / RULE_WIDTH;
	    push_symbol(buf, RULE(rule_mode, forbid), symbol_width, w);
	}
      do_last_sep:
	if (i < maxcol) {
	    forbid = 0;
	    if (type == T_TOP)
		forbid |= 2;
	    else if (t->tabattr[row][i + 1] & HTT_X) {
		forbid |= 2;
	    }
	    if (type == T_BOTTOM)
		forbid |= 8;
	    else {
		if (t->tabattr[row + 1][i + 1] & HTT_X) {
		    forbid |= 8;
		}
		if (t->tabattr[row + 1][i + 1] & HTT_Y) {
		    forbid |= 4;
		}
		if (t->tabattr[row + 1][i] & HTT_Y) {
		    forbid |= 1;
		}
	    }
	    if (forbid != 15)	/* forbid==15 means 'no rule at all' */
		push_symbol(buf, RULE(rule_mode, forbid), symbol_width, 1);
	}
    }
    forbid = 4;
    if (type == T_TOP)
	forbid |= 2;
    if (type == T_BOTTOM)
	forbid |= 8;
    if (t->tabattr[row + 1][maxcol] & HTT_Y) {
	forbid |= 1;
    }
    if (t->border_mode != BORDER_NOWIN)
	push_symbol(buf, RULE(t->border_mode, forbid), symbol_width, 1);
}