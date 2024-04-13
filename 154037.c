table_border_width(struct table *t)
{
    switch (t->border_mode) {
    case BORDER_THIN:
    case BORDER_THICK:
	return t->maxcol * t->cellspacing + 2 * (RULE_WIDTH + t->cellpadding);
    case BORDER_NOWIN:
    case BORDER_NONE:
	return t->maxcol * t->cellspacing;
    default:
	/* not reached */
	return 0;
    }
}