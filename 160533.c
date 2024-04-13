void print_usage(const char *pname)
{
	printf("Usage: %s [options] inputfiles\n", pname);
	printf("Options:\n"
		"\t-c\tcompact output\n"
		"\t-v\tverbose output\n"
		"\t-w\tprint white balance\n"
		"\t-u\tprint unpack function\n"
		"\t-f\tprint frame size (only w/ -u)\n"
		"\t-s\tprint output image size\n"
		"\t-h\tforce half-size mode (only for -s)\n"
		"\t-M\tdisable use of raw-embedded color data\n"
		"\t+M\tforce use of raw-embedded color data\n"
		"\t-L filename\tread input files list from filename\n"
		"\t-o filename\toutput to filename\n");
}