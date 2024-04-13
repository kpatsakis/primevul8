Module *PCM::createDecompress(_Track *track, AFvirtualfile *fh, bool canSeek,
	bool headerless, AFframecount *chunkframes)
{
	return new PCM(Decompress, track, fh, canSeek);
}