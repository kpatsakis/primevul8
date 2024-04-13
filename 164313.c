mime_header_encoder_new(
    enum mbfl_no_encoding incode,
    enum mbfl_no_encoding outcode,
    enum mbfl_no_encoding transenc)
{
	int n;
	const char *s;
	const mbfl_encoding *outencoding;
	struct mime_header_encoder_data *pe;

	/* get output encoding and check MIME charset name */
	outencoding = mbfl_no2encoding(outcode);
	if (outencoding == NULL || outencoding->mime_name == NULL || outencoding->mime_name[0] == '\0') {
		return NULL;
	}

	pe = (struct mime_header_encoder_data*)mbfl_malloc(sizeof(struct mime_header_encoder_data));
	if (pe == NULL) {
		return NULL;
	}

	mbfl_memory_device_init(&pe->outdev, 0, 0);
	mbfl_memory_device_init(&pe->tmpdev, 0, 0);
	pe->prevpos = 0;
	pe->linehead = 0;
	pe->firstindent = 0;
	pe->status1 = 0;
	pe->status2 = 0;

	/* make the encoding description string  exp. "=?ISO-2022-JP?B?" */
	n = 0;
	pe->encname[n++] = 0x3d;
	pe->encname[n++] = 0x3f;
	s = outencoding->mime_name;
	while (*s) {
		pe->encname[n++] = *s++;
	}
	pe->encname[n++] = 0x3f;
	if (transenc == mbfl_no_encoding_qprint) {
		pe->encname[n++] = 0x51;
	} else {
		pe->encname[n++] = 0x42;
		transenc = mbfl_no_encoding_base64;
	}
	pe->encname[n++] = 0x3f;
	pe->encname[n] = '\0';
	pe->encnamelen = n;

	n = 0;
	pe->lwsp[n++] = 0x0d;
	pe->lwsp[n++] = 0x0a;
	pe->lwsp[n++] = 0x20;
	pe->lwsp[n] = '\0';
	pe->lwsplen = n;

	/* transfer encode filter */
	pe->encod_filter = mbfl_convert_filter_new(outcode, transenc, mbfl_memory_device_output, 0, &(pe->outdev));
	pe->encod_filter_backup = mbfl_convert_filter_new(outcode, transenc, mbfl_memory_device_output, 0, &(pe->outdev));

	/* Output code filter */
	pe->conv2_filter = mbfl_convert_filter_new(mbfl_no_encoding_wchar, outcode, mbfl_filter_output_pipe, 0, pe->encod_filter);
	pe->conv2_filter_backup = mbfl_convert_filter_new(mbfl_no_encoding_wchar, outcode, mbfl_filter_output_pipe, 0, pe->encod_filter);

	/* encoded block filter */
	pe->block_filter = mbfl_convert_filter_new(mbfl_no_encoding_wchar, mbfl_no_encoding_wchar, mime_header_encoder_block_collector, 0, pe);

	/* Input code filter */
	pe->conv1_filter = mbfl_convert_filter_new(incode, mbfl_no_encoding_wchar, mime_header_encoder_collector, 0, pe);

	if (pe->encod_filter == NULL ||
	    pe->encod_filter_backup == NULL ||
	    pe->conv2_filter == NULL ||
	    pe->conv2_filter_backup == NULL ||
	    pe->conv1_filter == NULL) {
		mime_header_encoder_delete(pe);
		return NULL;
	}

	if (transenc == mbfl_no_encoding_qprint) {
		pe->encod_filter->status |= MBFL_QPRINT_STS_MIME_HEADER;
		pe->encod_filter_backup->status |= MBFL_QPRINT_STS_MIME_HEADER;
	} else {
		pe->encod_filter->status |= MBFL_BASE64_STS_MIME_HEADER;
		pe->encod_filter_backup->status |= MBFL_BASE64_STS_MIME_HEADER;
	}

	return pe;
}