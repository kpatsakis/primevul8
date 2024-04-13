mbfl_buffer_converter_getbuffer(mbfl_buffer_converter *convd, mbfl_string *result)
{
	if (convd != NULL && result != NULL && convd->device.buffer != NULL) {
		result->no_encoding = convd->to->no_encoding;
		result->val = convd->device.buffer;
		result->len = convd->device.pos;
	} else {
		result = NULL;
	}

	return result;
}