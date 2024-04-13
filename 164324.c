mime_header_encoder_result(struct mime_header_encoder_data *pe, mbfl_string *result)
{
	if (pe->status1 >= 10) {
		(*pe->conv2_filter->filter_flush)(pe->conv2_filter);
		(*pe->encod_filter->filter_flush)(pe->encod_filter);
		mbfl_memory_device_strncat(&pe->outdev, "\x3f\x3d", 2);		/* ?= */
	} else if (pe->tmpdev.pos > 0) {
		if (pe->outdev.pos > 0) {
			if ((pe->outdev.pos - pe->linehead + pe->tmpdev.pos) > 74) {
				mbfl_memory_device_strncat(&pe->outdev, pe->lwsp, pe->lwsplen);
			} else {
				mbfl_memory_device_output(0x20, &pe->outdev);
			}
		}
		mbfl_memory_device_devcat(&pe->outdev, &pe->tmpdev);
	}
	mbfl_memory_device_reset(&pe->tmpdev);
	pe->prevpos = 0;
	pe->linehead = 0;
	pe->status1 = 0;
	pe->status2 = 0;

	return mbfl_memory_device_result(&pe->outdev, result);
}