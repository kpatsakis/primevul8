minimum_cellspacing(int border_mode)
{
    switch (border_mode) {
    case BORDER_THIN:
    case BORDER_THICK:
    case BORDER_NOWIN:
	return RULE_WIDTH;
    case BORDER_NONE:
	return 1;
    default:
	/* not reached */
	return 0;
    }
}