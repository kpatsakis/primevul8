mime_header_encoder_collector(int c, void *data)
{
	static int qp_table[256] = {
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x00 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x00 */
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x20 */
		0, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 1, 0, 1, /* 0x10 */
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x40 */
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, /* 0x50 */
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x60 */
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, /* 0x70 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x80 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x90 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0xA0 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0xB0 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0xC0 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0xD0 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0xE0 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1  /* 0xF0 */
	};

	int n;
	struct mime_header_encoder_data *pe = (struct mime_header_encoder_data *)data;

	switch (pe->status1) {
	case 11:	/* encoded word */
		(*pe->block_filter->filter_function)(c, pe->block_filter);
		break;

	default:	/* ASCII */
		if (c <= 0x00ff && !qp_table[(c & 0xff)]) { /* ordinary characters */
			mbfl_memory_device_output(c, &pe->tmpdev);
			pe->status1 = 1;
		} else if (pe->status1 == 0 && c == 0x20) {	/* repeat SPACE */
			mbfl_memory_device_output(c, &pe->tmpdev);
		} else {
			if (pe->tmpdev.pos < 74 && c == 0x20) {
				n = pe->outdev.pos - pe->linehead + pe->tmpdev.pos + pe->firstindent;
				if (n > 74) {
					mbfl_memory_device_strncat(&pe->outdev, pe->lwsp, pe->lwsplen);		/* LWSP */
					pe->linehead = pe->outdev.pos;
					pe->firstindent = 0;
				} else if (pe->outdev.pos > 0) {
					mbfl_memory_device_output(0x20, &pe->outdev);
				}
				mbfl_memory_device_devcat(&pe->outdev, &pe->tmpdev);
				mbfl_memory_device_reset(&pe->tmpdev);
				pe->status1 = 0;
			} else {
				n = pe->outdev.pos - pe->linehead + pe->encnamelen + pe->firstindent;
				if (n > 60)  {
					mbfl_memory_device_strncat(&pe->outdev, pe->lwsp, pe->lwsplen);		/* LWSP */
					pe->linehead = pe->outdev.pos;
					pe->firstindent = 0;
				} else if (pe->outdev.pos > 0)  {
					mbfl_memory_device_output(0x20, &pe->outdev);
				}
				mbfl_convert_filter_devcat(pe->block_filter, &pe->tmpdev);
				mbfl_memory_device_reset(&pe->tmpdev);
				(*pe->block_filter->filter_function)(c, pe->block_filter);
				pe->status1 = 11;
			}
		}
		break;
	}

	return c;
}