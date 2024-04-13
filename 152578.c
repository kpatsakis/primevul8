exif_data_option_get_name (ExifDataOption o)
{
	unsigned int i;

	for (i = 0; exif_data_option[i].name; i++)
		if (exif_data_option[i].option == o) 
			break;
	return _(exif_data_option[i].name);
}