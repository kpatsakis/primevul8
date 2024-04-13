wav_set_chunk (SF_PRIVATE *psf, const SF_CHUNK_INFO * chunk_info)
{	return psf_save_write_chunk (&psf->wchunks, chunk_info) ;
} /* wav_set_chunk */