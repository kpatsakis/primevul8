mime_header_decoder_new(enum mbfl_no_encoding outcode)
{
	struct mime_header_decoder_data *pd;

	pd = (struct mime_header_decoder_data*)mbfl_malloc(sizeof(struct mime_header_decoder_data));
	if (pd == NULL) {
		return NULL;
	}

	mbfl_memory_device_init(&pd->outdev, 0, 0);
	mbfl_memory_device_init(&pd->tmpdev, 0, 0);
	pd->cspos = 0;
	pd->status = 0;
	pd->encoding = mbfl_no_encoding_pass;
	pd->incode = mbfl_no_encoding_ascii;
	pd->outcode = outcode;
	/* charset convert filter */
	pd->conv2_filter = mbfl_convert_filter_new(mbfl_no_encoding_wchar, pd->outcode, mbfl_memory_device_output, 0, &pd->outdev);
	pd->conv1_filter = mbfl_convert_filter_new(pd->incode, mbfl_no_encoding_wchar, mbfl_filter_output_pipe, 0, pd->conv2_filter);
	/* decode filter */
	pd->deco_filter = mbfl_convert_filter_new(pd->encoding, mbfl_no_encoding_8bit, mbfl_filter_output_pipe, 0, pd->conv1_filter);

	if (pd->conv1_filter == NULL || pd->conv2_filter == NULL || pd->deco_filter == NULL) {
		mime_header_decoder_delete(pd);
		return NULL;
	}

	return pd;
}