checkcmap(TIFF* tif, int n, uint16* r, uint16* g, uint16* b)
{
	while (n-- > 0)
		if (*r++ >= 256 || *g++ >= 256 || *b++ >= 256)
			return (16);
	TIFFWarning(TIFFFileName(tif), "Assuming 8-bit colormap");
	return (8);
}