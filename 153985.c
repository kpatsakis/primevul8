table_rule_width(struct table *t)
{
    if (t->border_mode == BORDER_NONE)
	return 1;
    return RULE_WIDTH;
}