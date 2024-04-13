write_rr_ER(struct archive_write *a)
{
	unsigned char *p;

	p = wb_buffptr(a);

	memset(p, 0, LOGICAL_BLOCK_SIZE);
	p[0] = 'E';
	p[1] = 'R';
	p[3] = 0x01;
	p[2] = RRIP_ER_SIZE;
	p[4] = RRIP_ER_ID_SIZE;
	p[5] = RRIP_ER_DSC_SIZE;
	p[6] = RRIP_ER_SRC_SIZE;
	p[7] = 0x01;
	memcpy(&p[8], rrip_identifier, p[4]);
	memcpy(&p[8+p[4]], rrip_descriptor, p[5]);
	memcpy(&p[8+p[4]+p[5]], rrip_source, p[6]);

	return (wb_consume(a, LOGICAL_BLOCK_SIZE));
}