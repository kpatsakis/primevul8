mime_header_decoder_collector(int c, void* data)
{
	const mbfl_encoding *encoding;
	struct mime_header_decoder_data *pd = (struct mime_header_decoder_data*)data;

	switch (pd->status) {
	case 1:
		if (c == 0x3f) {		/* ? */
			mbfl_memory_device_output(c, &pd->tmpdev);
			pd->cspos = pd->tmpdev.pos;
			pd->status = 2;
		} else {
			mbfl_convert_filter_devcat(pd->conv1_filter, &pd->tmpdev);
			mbfl_memory_device_reset(&pd->tmpdev);
			if (c == 0x3d) {		/* = */
				mbfl_memory_device_output(c, &pd->tmpdev);
			} else if (c == 0x0d || c == 0x0a) {	/* CR or LF */
				pd->status = 9;
			} else {
				(*pd->conv1_filter->filter_function)(c, pd->conv1_filter);
				pd->status = 0;
			}
		}
		break;
	case 2:		/* store charset string */
		if (c == 0x3f) {		/* ? */
			/* identify charset */
			mbfl_memory_device_output('\0', &pd->tmpdev);
			encoding = mbfl_name2encoding((const char *)&pd->tmpdev.buffer[pd->cspos]);
			if (encoding != NULL) {
				pd->incode = encoding->no_encoding;
				pd->status = 3;
			}
			mbfl_memory_device_unput(&pd->tmpdev);
			mbfl_memory_device_output(c, &pd->tmpdev);
		} else {
			mbfl_memory_device_output(c, &pd->tmpdev);
			if (pd->tmpdev.pos > 100) {		/* too long charset string */
				pd->status = 0;
			} else if (c == 0x0d || c == 0x0a) {	/* CR or LF */
				mbfl_memory_device_unput(&pd->tmpdev);
				pd->status = 9;
			}
			if (pd->status != 2) {
				mbfl_convert_filter_devcat(pd->conv1_filter, &pd->tmpdev);
				mbfl_memory_device_reset(&pd->tmpdev);
			}
		}
		break;
	case 3:		/* identify encoding */
		mbfl_memory_device_output(c, &pd->tmpdev);
		if (c == 0x42 || c == 0x62) {		/* 'B' or 'b' */
			pd->encoding = mbfl_no_encoding_base64;
			pd->status = 4;
		} else if (c == 0x51 || c == 0x71) {	/* 'Q' or 'q' */
			pd->encoding = mbfl_no_encoding_qprint;
			pd->status = 4;
		} else {
			if (c == 0x0d || c == 0x0a) {	/* CR or LF */
				mbfl_memory_device_unput(&pd->tmpdev);
				pd->status = 9;
			} else {
				pd->status = 0;
			}
			mbfl_convert_filter_devcat(pd->conv1_filter, &pd->tmpdev);
			mbfl_memory_device_reset(&pd->tmpdev);
		}
		break;
	case 4:		/* reset filter */
		mbfl_memory_device_output(c, &pd->tmpdev);
		if (c == 0x3f) {		/* ? */
			/* charset convert filter */
			mbfl_convert_filter_reset(pd->conv1_filter, pd->incode, mbfl_no_encoding_wchar);
			/* decode filter */
			mbfl_convert_filter_reset(pd->deco_filter, pd->encoding, mbfl_no_encoding_8bit);
			pd->status = 5;
		} else {
			if (c == 0x0d || c == 0x0a) {	/* CR or LF */
				mbfl_memory_device_unput(&pd->tmpdev);
				pd->status = 9;
			} else {
				pd->status = 0;
			}
			mbfl_convert_filter_devcat(pd->conv1_filter, &pd->tmpdev);
		}
		mbfl_memory_device_reset(&pd->tmpdev);
		break;
	case 5:		/* encoded block */
		if (c == 0x3f) {		/* ? */
			pd->status = 6;
		} else {
			(*pd->deco_filter->filter_function)(c, pd->deco_filter);
		}
		break;
	case 6:		/* check end position */
		if (c == 0x3d) {		/* = */
			/* flush and reset filter */
			(*pd->deco_filter->filter_flush)(pd->deco_filter);
			(*pd->conv1_filter->filter_flush)(pd->conv1_filter);
			mbfl_convert_filter_reset(pd->conv1_filter, mbfl_no_encoding_ascii, mbfl_no_encoding_wchar);
			pd->status = 7;
		} else {
			(*pd->deco_filter->filter_function)(0x3f, pd->deco_filter);
			if (c != 0x3f) {		/* ? */
				(*pd->deco_filter->filter_function)(c, pd->deco_filter);
				pd->status = 5;
			}
		}
		break;
	case 7:		/* after encoded block */
		if (c == 0x0d || c == 0x0a) {	/* CR LF */
			pd->status = 8;
		} else {
			mbfl_memory_device_output(c, &pd->tmpdev);
			if (c == 0x3d) {		/* = */
				pd->status = 1;
			} else if (c != 0x20 && c != 0x09) {		/* not space */
				mbfl_convert_filter_devcat(pd->conv1_filter, &pd->tmpdev);
				mbfl_memory_device_reset(&pd->tmpdev);
				pd->status = 0;
			}
		}
		break;
	case 8:		/* folding */
	case 9:		/* folding */
		if (c != 0x0d && c != 0x0a && c != 0x20 && c != 0x09) {
			if (c == 0x3d) {		/* = */
				if (pd->status == 8) {
					mbfl_memory_device_output(0x20, &pd->tmpdev);	/* SPACE */
				} else {
					(*pd->conv1_filter->filter_function)(0x20, pd->conv1_filter);
				}
				mbfl_memory_device_output(c, &pd->tmpdev);
				pd->status = 1;
			} else {
				mbfl_memory_device_output(0x20, &pd->tmpdev);
				mbfl_memory_device_output(c, &pd->tmpdev);
				mbfl_convert_filter_devcat(pd->conv1_filter, &pd->tmpdev);
				mbfl_memory_device_reset(&pd->tmpdev);
				pd->status = 0;
			}
		}
		break;
	default:		/* non encoded block */
		if (c == 0x0d || c == 0x0a) {	/* CR LF */
			pd->status = 9;
		} else if (c == 0x3d) {		/* = */
			mbfl_memory_device_output(c, &pd->tmpdev);
			pd->status = 1;
		} else {
			(*pd->conv1_filter->filter_function)(c, pd->conv1_filter);
		}
		break;
	}

	return c;
}