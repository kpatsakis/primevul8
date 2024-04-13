set_VD_bp(unsigned char *bp, enum VD_type type, unsigned char ver)
{

	/* Volume Descriptor Type */
	bp[1] = (unsigned char)type;
	/* Standard Identifier */
	memcpy(bp + 2, "CD001", 5);
	/* Volume Descriptor Version */
	bp[7] = ver;
}