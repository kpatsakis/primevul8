addcontentssize(struct table *t, int width)
{

    if (t->col < 0)
	return;
    if (t->tabwidth[t->col] < 0)
	return;
    check_row(t, t->row);
    t->tabcontentssize += width;
}