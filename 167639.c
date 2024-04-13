xsltFormatNumberConversion(xsltDecimalFormatPtr self,
			   xmlChar *format,
			   double number,
			   xmlChar **result)
{
    xmlXPathError status = XPATH_EXPRESSION_OK;
    xmlBufferPtr buffer;
    xmlChar *the_format, *prefix = NULL, *suffix = NULL;
    xmlChar *nprefix, *nsuffix = NULL;
    xmlChar pchar;
    int	    prefix_length, suffix_length = 0, nprefix_length, nsuffix_length;
    double  scale;
    int	    j, len;
    int     self_grouping_len;
    xsltFormatNumberInfo format_info;
    /*
     * delayed_multiplier allows a 'trailing' percent or
     * permille to be treated as suffix
     */
    int		delayed_multiplier = 0;
    /* flag to show no -ve format present for -ve number */
    char	default_sign = 0;
    /* flag to show error found, should use default format */
    char	found_error = 0;

    if (xmlStrlen(format) <= 0) {
	xsltTransformError(NULL, NULL, NULL,
                "xsltFormatNumberConversion : "
		"Invalid format (0-length)\n");
    }
    *result = NULL;
    switch (xmlXPathIsInf(number)) {
	case -1:
	    if (self->minusSign == NULL)
		*result = xmlStrdup(BAD_CAST "-");
	    else
		*result = xmlStrdup(self->minusSign);
	    /* no-break on purpose */
	case 1:
	    if ((self == NULL) || (self->infinity == NULL))
		*result = xmlStrcat(*result, BAD_CAST "Infinity");
	    else
		*result = xmlStrcat(*result, self->infinity);
	    return(status);
	default:
	    if (xmlXPathIsNaN(number)) {
		if ((self == NULL) || (self->noNumber == NULL))
		    *result = xmlStrdup(BAD_CAST "NaN");
		else
		    *result = xmlStrdup(self->noNumber);
		return(status);
	    }
    }

    buffer = xmlBufferCreate();
    if (buffer == NULL) {
	return XPATH_MEMORY_ERROR;
    }

    format_info.integer_hash = 0;
    format_info.integer_digits = 0;
    format_info.frac_digits = 0;
    format_info.frac_hash = 0;
    format_info.group = -1;
    format_info.multiplier = 1;
    format_info.add_decimal = FALSE;
    format_info.is_multiplier_set = FALSE;
    format_info.is_negative_pattern = FALSE;

    the_format = format;

    /*
     * First we process the +ve pattern to get percent / permille,
     * as well as main format
     */
    prefix = the_format;
    prefix_length = xsltFormatNumberPreSuffix(self, &the_format, &format_info);
    if (prefix_length < 0) {
	found_error = 1;
	goto OUTPUT_NUMBER;
    }

    /*
     * Here we process the "number" part of the format.  It gets
     * a little messy because of the percent/per-mille - if that
     * appears at the end, it may be part of the suffix instead
     * of part of the number, so the variable delayed_multiplier
     * is used to handle it
     */
    self_grouping_len = xmlStrlen(self->grouping);
    while ((*the_format != 0) &&
	   (xsltUTF8Charcmp(the_format, self->decimalPoint) != 0) &&
	   (xsltUTF8Charcmp(the_format, self->patternSeparator) != 0)) {

	if (delayed_multiplier != 0) {
	    format_info.multiplier = delayed_multiplier;
	    format_info.is_multiplier_set = TRUE;
	    delayed_multiplier = 0;
	}
	if (xsltUTF8Charcmp(the_format, self->digit) == 0) {
	    if (format_info.integer_digits > 0) {
		found_error = 1;
		goto OUTPUT_NUMBER;
	    }
	    format_info.integer_hash++;
	    if (format_info.group >= 0)
		format_info.group++;
	} else if (xsltUTF8Charcmp(the_format, self->zeroDigit) == 0) {
	    format_info.integer_digits++;
	    if (format_info.group >= 0)
		format_info.group++;
	} else if ((self_grouping_len > 0) &&
	    (!xmlStrncmp(the_format, self->grouping, self_grouping_len))) {
	    /* Reset group count */
	    format_info.group = 0;
	    the_format += self_grouping_len;
	    continue;
	} else if (xsltUTF8Charcmp(the_format, self->percent) == 0) {
	    if (format_info.is_multiplier_set) {
		found_error = 1;
		goto OUTPUT_NUMBER;
	    }
	    delayed_multiplier = 100;
	} else  if (xsltUTF8Charcmp(the_format, self->permille) == 0) {
	    if (format_info.is_multiplier_set) {
		found_error = 1;
		goto OUTPUT_NUMBER;
	    }
	    delayed_multiplier = 1000;
	} else
	    break; /* while */

	if ((len=xmlUTF8Strsize(the_format, 1)) < 1) {
	    found_error = 1;
	    goto OUTPUT_NUMBER;
	}
	the_format += len;

    }

    /* We have finished the integer part, now work on fraction */
    if ( (*the_format != 0) &&
         (xsltUTF8Charcmp(the_format, self->decimalPoint) == 0) ) {
        format_info.add_decimal = TRUE;
        if ((len = xmlUTF8Strsize(the_format, 1)) < 1) {
            found_error = 1;
            goto OUTPUT_NUMBER;
        }
	the_format += len;	/* Skip over the decimal */
    }

    while (*the_format != 0) {

	if (xsltUTF8Charcmp(the_format, self->zeroDigit) == 0) {
	    if (format_info.frac_hash != 0) {
		found_error = 1;
		goto OUTPUT_NUMBER;
	    }
	    format_info.frac_digits++;
	} else if (xsltUTF8Charcmp(the_format, self->digit) == 0) {
	    format_info.frac_hash++;
	} else if (xsltUTF8Charcmp(the_format, self->percent) == 0) {
	    if (format_info.is_multiplier_set) {
		found_error = 1;
		goto OUTPUT_NUMBER;
	    }
	    delayed_multiplier = 100;
	    if ((len = xmlUTF8Strsize(the_format, 1)) < 1) {
	        found_error = 1;
		goto OUTPUT_NUMBER;
	    }
	    the_format += len;
	    continue; /* while */
	} else if (xsltUTF8Charcmp(the_format, self->permille) == 0) {
	    if (format_info.is_multiplier_set) {
		found_error = 1;
		goto OUTPUT_NUMBER;
	    }
	    delayed_multiplier = 1000;
	    if  ((len = xmlUTF8Strsize(the_format, 1)) < 1) {
	        found_error = 1;
		goto OUTPUT_NUMBER;
	    }
	    the_format += len;
	    continue; /* while */
	} else if (xsltUTF8Charcmp(the_format, self->grouping) != 0) {
	    break; /* while */
	}
	if ((len = xmlUTF8Strsize(the_format, 1)) < 1) {
	    found_error = 1;
	    goto OUTPUT_NUMBER;
	}
	the_format += len;
	if (delayed_multiplier != 0) {
	    format_info.multiplier = delayed_multiplier;
	    delayed_multiplier = 0;
	    format_info.is_multiplier_set = TRUE;
	}
    }

    /*
     * If delayed_multiplier is set after processing the
     * "number" part, should be in suffix
     */
    if (delayed_multiplier != 0) {
	the_format -= len;
	delayed_multiplier = 0;
    }

    suffix = the_format;
    suffix_length = xsltFormatNumberPreSuffix(self, &the_format, &format_info);
    if ( (suffix_length < 0) ||
	 ((*the_format != 0) &&
	  (xsltUTF8Charcmp(the_format, self->patternSeparator) != 0)) ) {
	found_error = 1;
	goto OUTPUT_NUMBER;
    }

    /*
     * We have processed the +ve prefix, number part and +ve suffix.
     * If the number is -ve, we must substitute the -ve prefix / suffix
     */
    if (number < 0) {
        /*
	 * Note that j is the number of UTF8 chars before the separator,
	 * not the number of bytes! (bug 151975)
	 */
        j =  xmlUTF8Strloc(format, self->patternSeparator);
	if (j < 0) {
	/* No -ve pattern present, so use default signing */
	    default_sign = 1;
	}
	else {
	    /* Skip over pattern separator (accounting for UTF8) */
	    the_format = (xmlChar *)xmlUTF8Strpos(format, j + 1);
	    /*
	     * Flag changes interpretation of percent/permille
	     * in -ve pattern
	     */
	    format_info.is_negative_pattern = TRUE;
	    format_info.is_multiplier_set = FALSE;

	    /* First do the -ve prefix */
	    nprefix = the_format;
	    nprefix_length = xsltFormatNumberPreSuffix(self,
					&the_format, &format_info);
	    if (nprefix_length<0) {
		found_error = 1;
		goto OUTPUT_NUMBER;
	    }

	    while (*the_format != 0) {
		if ( (xsltUTF8Charcmp(the_format, (self)->percent) == 0) ||
		     (xsltUTF8Charcmp(the_format, (self)->permille)== 0) ) {
		    if (format_info.is_multiplier_set) {
			found_error = 1;
			goto OUTPUT_NUMBER;
		    }
		    format_info.is_multiplier_set = TRUE;
		    delayed_multiplier = 1;
		}
		else if (IS_SPECIAL(self, the_format))
		    delayed_multiplier = 0;
		else
		    break; /* while */
		if ((len = xmlUTF8Strsize(the_format, 1)) < 1) {
		    found_error = 1;
		    goto OUTPUT_NUMBER;
		}
		the_format += len;
	    }
	    if (delayed_multiplier != 0) {
		format_info.is_multiplier_set = FALSE;
		the_format -= len;
	    }

	    /* Finally do the -ve suffix */
	    if (*the_format != 0) {
		nsuffix = the_format;
		nsuffix_length = xsltFormatNumberPreSuffix(self,
					&the_format, &format_info);
		if (nsuffix_length < 0) {
		    found_error = 1;
		    goto OUTPUT_NUMBER;
		}
	    }
	    else
		nsuffix_length = 0;
	    if (*the_format != 0) {
		found_error = 1;
		goto OUTPUT_NUMBER;
	    }
	    /*
	     * Here's another Java peculiarity:
	     * if -ve prefix/suffix == +ve ones, discard & use default
	     */
	    if ((nprefix_length != prefix_length) ||
		(nsuffix_length != suffix_length) ||
		((nprefix_length > 0) &&
		 (xmlStrncmp(nprefix, prefix, prefix_length) !=0 )) ||
		((nsuffix_length > 0) &&
		 (xmlStrncmp(nsuffix, suffix, suffix_length) !=0 ))) {
		prefix = nprefix;
		prefix_length = nprefix_length;
		suffix = nsuffix;
		suffix_length = nsuffix_length;
	    } /* else {
		default_sign = 1;
	    }
	    */
	}
    }

OUTPUT_NUMBER:
    if (found_error != 0) {
	xsltTransformError(NULL, NULL, NULL,
                "xsltFormatNumberConversion : "
		"error in format string '%s', using default\n", format);
	default_sign = (number < 0.0) ? 1 : 0;
	prefix_length = suffix_length = 0;
	format_info.integer_hash = 0;
	format_info.integer_digits = 1;
	format_info.frac_digits = 1;
	format_info.frac_hash = 4;
	format_info.group = -1;
	format_info.multiplier = 1;
	format_info.add_decimal = TRUE;
    }

    /* Ready to output our number.  First see if "default sign" is required */
    if (default_sign != 0)
	xmlBufferAdd(buffer, self->minusSign, xmlUTF8Strsize(self->minusSign, 1));

    /* Put the prefix into the buffer */
    for (j = 0; j < prefix_length; j++) {
	if ((pchar = *prefix++) == SYMBOL_QUOTE) {
	    len = xmlUTF8Strsize(prefix, 1);
	    xmlBufferAdd(buffer, prefix, len);
	    prefix += len;
	    j += len - 1;	/* length of symbol less length of quote */
	} else
	    xmlBufferAdd(buffer, &pchar, 1);
    }

    /* Next do the integer part of the number */
    number = fabs(number) * (double)format_info.multiplier;
    scale = pow(10.0, (double)(format_info.frac_digits + format_info.frac_hash));
    number = floor((scale * number + 0.5)) / scale;
    if ((self->grouping != NULL) &&
        (self->grouping[0] != 0)) {
        int gchar;

	len = xmlStrlen(self->grouping);
	gchar = xsltGetUTF8Char(self->grouping, &len);
	xsltNumberFormatDecimal(buffer, floor(number), self->zeroDigit[0],
				format_info.integer_digits,
				format_info.group,
				gchar, len);
    } else
	xsltNumberFormatDecimal(buffer, floor(number), self->zeroDigit[0],
				format_info.integer_digits,
				format_info.group,
				',', 1);

    /* Special case: java treats '.#' like '.0', '.##' like '.0#', etc. */
    if ((format_info.integer_digits + format_info.integer_hash +
	 format_info.frac_digits == 0) && (format_info.frac_hash > 0)) {
        ++format_info.frac_digits;
	--format_info.frac_hash;
    }

    /* Add leading zero, if required */
    if ((floor(number) == 0) &&
	(format_info.integer_digits + format_info.frac_digits == 0)) {
        xmlBufferAdd(buffer, self->zeroDigit, xmlUTF8Strsize(self->zeroDigit, 1));
    }

    /* Next the fractional part, if required */
    if (format_info.frac_digits + format_info.frac_hash == 0) {
        if (format_info.add_decimal)
	    xmlBufferAdd(buffer, self->decimalPoint,
			 xmlUTF8Strsize(self->decimalPoint, 1));
    }
    else {
      number -= floor(number);
	if ((number != 0) || (format_info.frac_digits != 0)) {
	    xmlBufferAdd(buffer, self->decimalPoint,
			 xmlUTF8Strsize(self->decimalPoint, 1));
	    number = floor(scale * number + 0.5);
	    for (j = format_info.frac_hash; j > 0; j--) {
		if (fmod(number, 10.0) >= 1.0)
		    break; /* for */
		number /= 10.0;
	    }
	    xsltNumberFormatDecimal(buffer, floor(number), self->zeroDigit[0],
				format_info.frac_digits + j,
				0, 0, 0);
	}
    }
    /* Put the suffix into the buffer */
    for (j = 0; j < suffix_length; j++) {
	if ((pchar = *suffix++) == SYMBOL_QUOTE) {
            len = xmlUTF8Strsize(suffix, 1);
	    xmlBufferAdd(buffer, suffix, len);
	    suffix += len;
	    j += len - 1;	/* length of symbol less length of escape */
	} else
	    xmlBufferAdd(buffer, &pchar, 1);
    }

    *result = xmlStrdup(xmlBufferContent(buffer));
    xmlBufferFree(buffer);
    return status;
}