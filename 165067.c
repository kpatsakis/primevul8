Module *IMA::createDecompress(_Track *track, AFvirtualfile *fh, bool canSeek,
	bool headerless, AFframecount *chunkFrames)
{
	assert(fh->tell() == track->fpos_first_frame);

	IMA *ima = new IMA(track, fh, canSeek);

	AUpvlist pv = (AUpvlist) track->f.compressionParams;

	long l;
	if (_af_pv_getlong(pv, _AF_FRAMES_PER_BLOCK, &l))
		ima->m_framesPerBlock = l;
	else
		_af_error(AF_BAD_CODEC_CONFIG, "samples per block not set");

	if (_af_pv_getlong(pv, _AF_BLOCK_SIZE, &l))
		ima->m_blockAlign = l;
	else
		_af_error(AF_BAD_CODEC_CONFIG, "block size not set");

	*chunkFrames = ima->m_framesPerBlock;

	return ima;
}