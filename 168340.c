_XimCountNumberOfAttr(
    CARD16	  total,
    CARD16	 *attr,
    unsigned int *names_len)
{
    unsigned int n;
    CARD16	 len;
    CARD16	 min_len = sizeof(CARD16)	/* sizeof attribute ID */
			 + sizeof(CARD16)	/* sizeof type of value */
			 + sizeof(INT16);	/* sizeof length of attribute */

    n = 0;
    *names_len = 0;
    while (total > min_len) {
	len = attr[2];
	if (len >= (total - min_len)) {
	    return 0;
	}
	*names_len += (len + 1);
	len += (min_len + XIM_PAD(len + 2));
	total -= len;
	attr = (CARD16 *)((char *)attr + len);
	n++;
    }
    return n;
}