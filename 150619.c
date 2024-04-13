static int next_segment(unsigned long len, int offset)
{
	if (len > PAGE_SIZE - offset)
		return PAGE_SIZE - offset;
	else
		return len;
}