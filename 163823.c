wav_get_chunk_size (SF_PRIVATE *psf, const SF_CHUNK_ITERATOR * iterator, SF_CHUNK_INFO * chunk_info)
{	int indx ;

	if ((indx = psf_find_read_chunk_iterator (&psf->rchunks, iterator)) < 0)
		return SFE_UNKNOWN_CHUNK ;

	chunk_info->datalen = psf->rchunks.chunks [indx].len ;

	return SFE_NO_ERROR ;
} /* wav_get_chunk_size */