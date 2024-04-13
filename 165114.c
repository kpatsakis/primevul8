Module *_af_ms_adpcm_init_decompress (_Track *track, AFvirtualfile *fh,
	bool canSeek, bool headerless, AFframecount *chunkFrames)
{
	return MSADPCM::createDecompress(track, fh, canSeek, headerless, chunkFrames);
}