void PCM::sync2()
{
	assert(!canSeek() || (tell() == m_track->fpos_next_frame));

	/* We can afford to seek because sync2 is rare. */
	m_track->fpos_after_data = tell();

	m_track->fpos_next_frame = m_saved_fpos_next_frame;
	m_track->nextfframe = m_saved_nextfframe;
}