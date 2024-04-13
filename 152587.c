cmp_func_motorola (const void *elem1, const void *elem2)
{
	return cmp_func ((const unsigned char *) elem1,
			 (const unsigned char *) elem2, EXIF_BYTE_ORDER_MOTOROLA);
}