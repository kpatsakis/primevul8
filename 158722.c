static int find_stream_bounds(const char *start, off_t bytesleft, off_t bytesleft2, off_t *stream, off_t *endstream)
{
    const char *q2, *q;
    if ((q2 = cli_memstr(start, bytesleft, "stream", 6))) {
	q2 += 6;
	if (q2[0] == '\xd' && q2[1] == '\xa')
	    q2 += 2;
	if (q2[0] == '\xa')
	    q2++;
	*stream = q2 - start;
	bytesleft2 -= q2 - start;
	q = q2;
	q2 = cli_memstr(q, bytesleft2, "endstream", 9);
	if (!q2)
	    return 0;/* no more objs */
	*endstream = q2 - start;
	return 1;
    }
    return 0;
}