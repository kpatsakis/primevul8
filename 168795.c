static const char* channel_response_fields_present_to_string(UINT16 fieldsPresent)
{
	return fields_present_to_string(fieldsPresent, channel_response_fields_present,
	                                ARRAYSIZE(channel_response_fields_present));
}