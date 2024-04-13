static int next_pending_bucket(struct timer_base *base, unsigned offset,
			       unsigned clk)
{
	unsigned pos, start = offset + clk;
	unsigned end = offset + LVL_SIZE;

	pos = find_next_bit(base->pending_map, end, start);
	if (pos < end)
		return pos - start;

	pos = find_next_bit(base->pending_map, start, offset);
	return pos < start ? pos + LVL_SIZE - start : -1;
}