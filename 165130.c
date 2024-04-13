Module *PCM::createCompress(_Track *track, AFvirtualfile *fh, bool canSeek,
	bool headerless, AFframecount *chunkframes)
{
	return new PCM(Compress, track, fh, canSeek);
}