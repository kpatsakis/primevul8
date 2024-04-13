G711::G711(Mode mode, _Track *track, AFvirtualfile *fh, bool canSeek) :
	FileModule(mode, track, fh, canSeek),
	m_savedPositionNextFrame(-1),
	m_savedNextFrame(-1)
{
	if (mode == Decompress)
		track->f.compressionParams = AU_NULL_PVLIST;
}