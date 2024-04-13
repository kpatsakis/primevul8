Module *MSADPCM::createDecompress(_Track *track, AFvirtualfile *fh,
	bool canSeek, bool headerless, AFframecount *chunkFrames)
{
	assert(fh->tell() == track->fpos_first_frame);

	MSADPCM *msadpcm = new MSADPCM(track, fh, canSeek);

	AUpvlist pv = (AUpvlist) track->f.compressionParams;
	long l;
	if (_af_pv_getlong(pv, _AF_MS_ADPCM_NUM_COEFFICIENTS, &l))
		msadpcm->m_numCoefficients = l;
	else
		_af_error(AF_BAD_CODEC_CONFIG, "number of coefficients not set");

	void *v;
	if (_af_pv_getptr(pv, _AF_MS_ADPCM_COEFFICIENTS, &v))
		memcpy(msadpcm->m_coefficients, v, sizeof (int16_t) * 2 * msadpcm->m_numCoefficients);
	else
		_af_error(AF_BAD_CODEC_CONFIG, "coefficient array not set");

	if (_af_pv_getlong(pv, _AF_FRAMES_PER_BLOCK, &l))
		msadpcm->m_framesPerBlock = l;
	else
		_af_error(AF_BAD_CODEC_CONFIG, "samples per block not set");

	if (_af_pv_getlong(pv, _AF_BLOCK_SIZE, &l))
		msadpcm->m_blockAlign = l;
	else
		_af_error(AF_BAD_CODEC_CONFIG, "block size not set");

	*chunkFrames = msadpcm->m_framesPerBlock;

	return msadpcm;
}