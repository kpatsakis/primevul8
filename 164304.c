filter_count_output(int c, void *data)
{
	(*(int *)data)++;
	return c;
}