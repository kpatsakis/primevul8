static const char* tunnel_response_fields_present_to_string(UINT16 fieldsPresent)
{
	return fields_present_to_string(fieldsPresent, tunnel_response_fields_present,
	                                ARRAYSIZE(tunnel_response_fields_present));
}