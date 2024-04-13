PCM::PCM(Mode mode, _Track *track, AFvirtualfile *fh, bool canSeek) :
	FileModule(mode, track, fh, canSeek),
	m_bytesPerFrame(track->f.bytesPerFrame(false)),
	m_saved_fpos_next_frame(-1),
	m_saved_nextfframe(-1)
{
	if (mode == Decompress)
		track->f.compressionParams = AU_NULL_PVLIST;
}