void IMA::reset2()
{
	m_track->fpos_next_frame = m_track->fpos_first_frame +
		m_blockAlign * (m_track->nextfframe / m_framesPerBlock);
	m_track->frames2ignore += m_framesToIgnore;

	assert(m_track->nextfframe % m_framesPerBlock == 0);
}