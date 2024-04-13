xsltFormatNumberPreSuffix(xsltDecimalFormatPtr self, xmlChar **format, xsltFormatNumberInfoPtr info)
{
    int	count=0;	/* will hold total length of prefix/suffix */
    int len;

    while (1) {
	/*
	 * prefix / suffix ends at end of string or at
	 * first 'special' character
	 */
	if (**format == 0)
	    return count;
	/* if next character 'escaped' just count it */
	if (**format == SYMBOL_QUOTE) {
	    if (*++(*format) == 0)
		return -1;
	}
	else if (IS_SPECIAL(self, *format))
	    return count;
	/*
	 * else treat percent/per-mille as special cases,
	 * depending on whether +ve or -ve
	 */
	else {
	    /*
	     * for +ve prefix/suffix, allow only a
	     * single occurence of either
	     */
	    if (xsltUTF8Charcmp(*format, self->percent) == 0) {
		if (info->is_multiplier_set)
		    return -1;
		info->multiplier = 100;
		info->is_multiplier_set = TRUE;
	    } else if (xsltUTF8Charcmp(*format, self->permille) == 0) {
		if (info->is_multiplier_set)
		    return -1;
		info->multiplier = 1000;
		info->is_multiplier_set = TRUE;
	    }
	}

	if ((len=xmlUTF8Strsize(*format, 1)) < 1)
	    return -1;
	count += len;
	*format += len;
    }
}