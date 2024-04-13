mbfl_substr(
    mbfl_string *string,
    mbfl_string *result,
    int from,
    int length)
{
	const mbfl_encoding *encoding;
	int n, m, k, len, start, end;
	unsigned char *p, *w;
	const unsigned char *mbtab;

	encoding = mbfl_no2encoding(string->no_encoding);
	if (encoding == NULL || string == NULL || result == NULL) {
		return NULL;
	}
	mbfl_string_init(result);
	result->no_language = string->no_language;
	result->no_encoding = string->no_encoding;

	if ((encoding->flag & (MBFL_ENCTYPE_SBCS | MBFL_ENCTYPE_WCS2BE | MBFL_ENCTYPE_WCS2LE | MBFL_ENCTYPE_WCS4BE | MBFL_ENCTYPE_WCS4LE)) ||
	   encoding->mblen_table != NULL) {
		len = string->len;
		start = from;
		end = from + length;
		if (encoding->flag & (MBFL_ENCTYPE_WCS2BE | MBFL_ENCTYPE_WCS2LE)) {
			start *= 2;
			end = start + length*2;
		} else if (encoding->flag & (MBFL_ENCTYPE_WCS4BE | MBFL_ENCTYPE_WCS4LE)) {
			start *= 4;
			end = start + length*4;
		} else if (encoding->mblen_table != NULL) {
			mbtab = encoding->mblen_table;
			start = 0;
			end = 0;
			n = 0;
			k = 0;
			p = string->val;
			if (p != NULL) {
				/* search start position */
				while (k <= from) {
					start = n;
					if (n >= len) {
						break;
					}
					m = mbtab[*p];
					n += m;
					p += m;
					k++;
				}
				/* detect end position */
				k = 0;
				end = start;
				while (k < length) {
					end = n;
					if (n >= len) {
						break;
					}
					m = mbtab[*p];
					n += m;
					p += m;
					k++;
				}
			}
		}

		if (start > len) {
			start = len;
		}
		if (start < 0) {
			start = 0;
		}
		if (end > len) {
			end = len;
		}
		if (end < 0) {
			end = 0;
		}
		if (start > end) {
			start = end;
		}

		/* allocate memory and copy */
		n = end - start;
		result->len = 0;
		result->val = w = (unsigned char*)mbfl_malloc((n + 8)*sizeof(unsigned char));
		if (w != NULL) {
			p = string->val;
			if (p != NULL) {
				p += start;
				result->len = n;
				while (n > 0) {
					*w++ = *p++;
					n--;
				}
			}
			*w++ = '\0';
			*w++ = '\0';
			*w++ = '\0';
			*w = '\0';
		} else {
			result = NULL;
		}
	} else {
		mbfl_memory_device device;
		struct collector_substr_data pc;
		mbfl_convert_filter *decoder;
		mbfl_convert_filter *encoder;

		mbfl_memory_device_init(&device, length + 1, 0);
		mbfl_string_init(result);
		result->no_language = string->no_language;
		result->no_encoding = string->no_encoding;
		/* output code filter */
		decoder = mbfl_convert_filter_new(
		    mbfl_no_encoding_wchar,
		    string->no_encoding,
		    mbfl_memory_device_output, 0, &device);
		/* wchar filter */
		encoder = mbfl_convert_filter_new(
		    string->no_encoding,
		    mbfl_no_encoding_wchar,
		    collector_substr, 0, &pc);
		if (decoder == NULL || encoder == NULL) {
			mbfl_convert_filter_delete(encoder);
			mbfl_convert_filter_delete(decoder);
			return NULL;
		}
		pc.next_filter = decoder;
		pc.start = from;
		pc.stop = from + length;
		pc.output = 0;

		/* feed data */
		p = string->val;
		n = string->len;
		if (p != NULL) {
			while (n > 0) {
				if ((*encoder->filter_function)(*p++, encoder) < 0) {
					break;
				}
				n--;
			}
		}

		mbfl_convert_filter_flush(encoder);
		mbfl_convert_filter_flush(decoder);
		result = mbfl_memory_device_result(&device, result);
		mbfl_convert_filter_delete(encoder);
		mbfl_convert_filter_delete(decoder);
	}

	return result;
}