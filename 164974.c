MSADPCM::MSADPCM(_Track *track, AFvirtualfile *fh, bool canSeek) :
	FileModule(Decompress, track, fh, canSeek)
{
}