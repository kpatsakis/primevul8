void PCM::reset2()
{
	m_track->fpos_next_frame = m_track->fpos_first_frame +
		m_bytesPerFrame * m_track->nextfframe;

	m_track->frames2ignore = 0;
}