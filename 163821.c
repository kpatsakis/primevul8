wav_next_chunk_iterator (SF_PRIVATE *psf, SF_CHUNK_ITERATOR * iterator)
{	return psf_next_chunk_iterator (&psf->rchunks, iterator) ;
} /* wav_next_chunk_iterator */