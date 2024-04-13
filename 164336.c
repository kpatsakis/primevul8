mime_header_decoder_result(struct mime_header_decoder_data *pd, mbfl_string *result)
{
	switch (pd->status) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 7:
	case 8:
	case 9:
		mbfl_convert_filter_devcat(pd->conv1_filter, &pd->tmpdev);
		break;
	case 5:
	case 6:
		(*pd->deco_filter->filter_flush)(pd->deco_filter);
		(*pd->conv1_filter->filter_flush)(pd->conv1_filter);
		break;
	}
	(*pd->conv2_filter->filter_flush)(pd->conv2_filter);
	mbfl_memory_device_reset(&pd->tmpdev);
	pd->status = 0;

	return mbfl_memory_device_result(&pd->outdev, result);
}