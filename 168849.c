static const char* fields_present_to_string(UINT16 fieldsPresent, const t_err_mapping* map,
                                            size_t elements)
{
	size_t x = 0;
	static char buffer[1024] = { 0 };
	char fields[12];
	memset(buffer, 0, sizeof(buffer));

	for (x = 0; x < elements; x++)
	{
		if (buffer[0] != '\0')
			strcat(buffer, "|");

		if ((map[x].code & fieldsPresent) != 0)
			strcat(buffer, map[x].name);
	}

	sprintf_s(fields, ARRAYSIZE(fields), " [%04" PRIx16 "]", fieldsPresent);
	strcat(buffer, fields);
	return buffer;
}