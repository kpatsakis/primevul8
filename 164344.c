filter_count_width(int c, void* data)
{
	(*(int *)data) += (is_fullwidth(c) ? 2: 1);
	return c;
}