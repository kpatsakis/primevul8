Module *G711::createDecompress(_Track *track, AFvirtualfile *fh,
	bool canSeek, bool headerless, AFframecount *chunkframes)
{
	return new G711(Decompress, track, fh, canSeek);
}