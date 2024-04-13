void usage(const char *av)
{
	printf(
		"Dump (small) selecton of RAW file as tab-separated text file\n"
		"Usage: %s inputfile COL ROW [CHANNEL] [width] [height]\n"
		"  COL - start column\n"
		"  ROW - start row\n"
		"  CHANNEL - raw channel to dump, default is 0 (red for rggb)\n"
		"  width - area width to dump, default is 16\n"
		"  height - area height to dump, default is 4\n"
		, av);
}