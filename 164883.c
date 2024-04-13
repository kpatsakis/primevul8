Module *_AFg711initcompress(_Track *track, AFvirtualfile *fh, bool canSeek,
	bool headerless, AFframecount *chunkFrames)
{
	return G711::createCompress(track, fh, canSeek, headerless, chunkFrames);
}