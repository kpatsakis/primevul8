mbfl_strimwidth(
    mbfl_string *string,
    mbfl_string *marker,
    mbfl_string *result,
    int from,
    int width)
{
	struct collector_strimwidth_data pc;
	mbfl_convert_filter *encoder;
	int n, mkwidth;
	unsigned char *p;

	if (string == NULL || result == NULL) {
		return NULL;
	}
	mbfl_string_init(result);
	result->no_language = string->no_language;
	result->no_encoding = string->no_encoding;
	mbfl_memory_device_init(&pc.device, width, 0);

	/* output code filter */
	pc.decoder = mbfl_convert_filter_new(
	    mbfl_no_encoding_wchar,
	    string->no_encoding,
	    mbfl_memory_device_output, 0, &pc.device);
	pc.decoder_backup = mbfl_convert_filter_new(
	    mbfl_no_encoding_wchar,
	    string->no_encoding,
	    mbfl_memory_device_output, 0, &pc.device);
	/* wchar filter */
	encoder = mbfl_convert_filter_new(
	    string->no_encoding,
	    mbfl_no_encoding_wchar,
	    collector_strimwidth, 0, &pc);
	if (pc.decoder == NULL || pc.decoder_backup == NULL || encoder == NULL) {
		mbfl_convert_filter_delete(encoder);
		mbfl_convert_filter_delete(pc.decoder);
		mbfl_convert_filter_delete(pc.decoder_backup);
		return NULL;
	}
	mkwidth = 0;
	if (marker) {
		mkwidth = mbfl_strwidth(marker);
	}
	pc.from = from;
	pc.width = width - mkwidth;
	pc.outwidth = 0;
	pc.outchar = 0;
	pc.status = 0;
	pc.endpos = 0;

	/* feed data */
	p = string->val;
	n = string->len;
	if (p != NULL) {
		while (n > 0) {
			n--;
			if ((*encoder->filter_function)(*p++, encoder) < 0) {
				break;
			}
		}
		mbfl_convert_filter_flush(encoder);
		if (pc.status != 0 && mkwidth > 0) {
			pc.width += mkwidth;
			while (n > 0) {
				if ((*encoder->filter_function)(*p++, encoder) < 0) {
					break;
				}
				n--;
			}
			mbfl_convert_filter_flush(encoder);
			if (pc.status != 1) {
				pc.status = 10;
				pc.device.pos = pc.endpos;
				mbfl_convert_filter_copy(pc.decoder_backup, pc.decoder);
				mbfl_convert_filter_reset(encoder, marker->no_encoding, mbfl_no_encoding_wchar);
				p = marker->val;
				n = marker->len;
				while (n > 0) {
					if ((*encoder->filter_function)(*p++, encoder) < 0) {
						break;
					}
					n--;
				}
				mbfl_convert_filter_flush(encoder);
			}
		} else if (pc.status != 0) {
			pc.device.pos = pc.endpos;
			mbfl_convert_filter_copy(pc.decoder_backup, pc.decoder);
		}
		mbfl_convert_filter_flush(pc.decoder);
	}
	result = mbfl_memory_device_result(&pc.device, result);
	mbfl_convert_filter_delete(encoder);
	mbfl_convert_filter_delete(pc.decoder);
	mbfl_convert_filter_delete(pc.decoder_backup);

	return result;
}