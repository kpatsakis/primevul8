mime_header_encoder_delete(struct mime_header_encoder_data *pe)
{
	if (pe) {
		mbfl_convert_filter_delete(pe->conv1_filter);
		mbfl_convert_filter_delete(pe->block_filter);
		mbfl_convert_filter_delete(pe->conv2_filter);
		mbfl_convert_filter_delete(pe->conv2_filter_backup);
		mbfl_convert_filter_delete(pe->encod_filter);
		mbfl_convert_filter_delete(pe->encod_filter_backup);
		mbfl_memory_device_clear(&pe->outdev);
		mbfl_memory_device_clear(&pe->tmpdev);
		mbfl_free((void*)pe);
	}
}