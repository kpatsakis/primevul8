cmp_func (const unsigned char *p1, const unsigned char *p2, ExifByteOrder o)
{
	ExifShort tag1 = exif_get_short (p1, o);
	ExifShort tag2 = exif_get_short (p2, o);

	return (tag1 < tag2) ? -1 : (tag1 > tag2) ? 1 : 0;
}