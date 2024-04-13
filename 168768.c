static const char* tunnel_authorization_response_fields_present_to_string(UINT16 fieldsPresent)
{
	return fields_present_to_string(fieldsPresent, tunnel_authorization_response_fields_present,
	                                ARRAYSIZE(tunnel_authorization_response_fields_present));
}