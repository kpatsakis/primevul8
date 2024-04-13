static const char *findNextNonWSBack(const char *q, const char *start)
{
    while (q > start &&
	   (*q == 0 || *q == 9 || *q == 0xa || *q == 0xc || *q == 0xd || *q == 0x20))
    {
	q--;
    }
    return q;
}