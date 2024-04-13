void SetITUFax(j_compress_ptr cinfo)
{
	unsigned char Marker[] = "G3FAX\x00\0x07\xCA\x00\xC8";

	jpeg_write_marker(cinfo, (JPEG_APP0 + 1), Marker, 10);
}