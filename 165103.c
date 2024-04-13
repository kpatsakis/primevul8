Module *_AFpcminitcompress (_Track *track, AFvirtualfile *fh, bool canSeek,
	bool headerless, AFframecount *chunkFrames)
{
	return PCM::createCompress(track, fh, canSeek, headerless, chunkFrames);
}