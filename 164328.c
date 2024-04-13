mbfl_buffer_converter_result(mbfl_buffer_converter *convd, mbfl_string *result)
{
	if (convd == NULL || result == NULL) {
		return NULL;
	}
	result->no_encoding = convd->to->no_encoding;
	return mbfl_memory_device_result(&convd->device, result);
}