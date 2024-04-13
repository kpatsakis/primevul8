FileModule::FileModule(Mode mode, _Track *track, AFvirtualfile *fh, bool canSeek) :
	m_mode(mode),
	m_track(track),
	m_fh(fh),
	m_canSeek(canSeek)
{
	track->fpos_next_frame = track->fpos_first_frame;
	track->frames2ignore = 0;
}