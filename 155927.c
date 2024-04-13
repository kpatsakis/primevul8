PHP_MINFO_FUNCTION(exif)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "EXIF Support", "enabled");
	php_info_print_table_row(2, "EXIF Version", PHP_EXIF_VERSION);
	php_info_print_table_row(2, "Supported EXIF Version", "0220");
	php_info_print_table_row(2, "Supported filetypes", "JPEG,TIFF");
	php_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}