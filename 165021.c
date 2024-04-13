IMA::IMA(_Track *track, AFvirtualfile *fh, bool canSeek) :
	FileModule(Decompress, track, fh, canSeek),
	m_blockAlign(-1),
	m_framesPerBlock(-1),
	m_framesToIgnore(-1)
{
}