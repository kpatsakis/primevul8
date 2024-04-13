wav_get_chunk_data (SF_PRIVATE *psf, const SF_CHUNK_ITERATOR * iterator, SF_CHUNK_INFO * chunk_info)
{	int indx ;
	sf_count_t pos ;

	if ((indx = psf_find_read_chunk_iterator (&psf->rchunks, iterator)) < 0)
		return SFE_UNKNOWN_CHUNK ;

	if (chunk_info->data == NULL)
		return SFE_BAD_CHUNK_DATA_PTR ;

	chunk_info->id_size = psf->rchunks.chunks [indx].id_size ;
	memcpy (chunk_info->id, psf->rchunks.chunks [indx].id, sizeof (chunk_info->id) / sizeof (*chunk_info->id)) ;

	pos = psf_ftell (psf) ;
	psf_fseek (psf, psf->rchunks.chunks [indx].offset, SEEK_SET) ;
	psf_fread (chunk_info->data, SF_MIN (chunk_info->datalen, psf->rchunks.chunks [indx].len), 1, psf) ;
	psf_fseek (psf, pos, SEEK_SET) ;

	return SFE_NO_ERROR ;
} /* wav_get_chunk_data */