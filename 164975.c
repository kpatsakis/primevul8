Module *_AFpcminitdecompress (_Track *track, AFvirtualfile *fh, bool canSeek,
	bool headerless, AFframecount *chunkFrames)
{
	return PCM::createDecompress(track, fh, canSeek, headerless, chunkFrames);
}