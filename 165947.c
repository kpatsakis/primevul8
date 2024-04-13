psf_decode_frame_count (SF_PRIVATE *psf)
{	sf_count_t count, readlen, total = 0 ;
	BUF_UNION	ubuf ;

	/* If we're reading from a pipe or the file is too long, just return SF_COUNT_MAX. */
	if (psf_is_pipe (psf) || psf->datalength > 0x1000000)
		return SF_COUNT_MAX ;

	psf_fseek (psf, psf->dataoffset, SEEK_SET) ;

	readlen = ARRAY_LEN (ubuf.ibuf) / psf->sf.channels ;
	readlen *= psf->sf.channels ;

	while ((count = psf->read_int (psf, ubuf.ibuf, readlen)) > 0)
		total += count ;

	psf_fseek (psf, psf->dataoffset, SEEK_SET) ;

	return total / psf->sf.channels ;
} /* psf_decode_frame_count */