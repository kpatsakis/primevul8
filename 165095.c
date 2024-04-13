Module *G711::createCompress(_Track *track, AFvirtualfile *fh,
	bool canSeek, bool headerless, AFframecount *chunkframes)
{
	return new G711(Compress, track, fh, canSeek);
}