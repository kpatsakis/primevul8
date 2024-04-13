write_VD_terminator(struct archive_write *a)
{
	unsigned char *bp;

	bp = wb_buffptr(a) -1;
	set_VD_bp(bp, VDT_TERMINATOR, 1);
	set_unused_field_bp(bp, 8, LOGICAL_BLOCK_SIZE);

	return (wb_consume(a, LOGICAL_BLOCK_SIZE));
}