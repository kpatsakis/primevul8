Module *_af_ima_adpcm_init_decompress(_Track *track, AFvirtualfile *fh,
	bool canSeek, bool headerless, AFframecount *chunkFrames)
{
	return IMA::createDecompress(track, fh, canSeek, headerless, chunkFrames);
}