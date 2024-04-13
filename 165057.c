Module *_AFg711initdecompress(_Track *track, AFvirtualfile *fh, bool canSeek,
	bool headerless, AFframecount *chunkFrames)
{
	return G711::createDecompress(track, fh, canSeek, headerless, chunkFrames);
}