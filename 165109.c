void G711::sync2()
{
	/* sanity check. */
	assert(!canSeek() || (tell() == m_track->fpos_next_frame));

	/* We can afford to do an lseek just in case because sync2 is rare. */
	m_track->fpos_after_data = tell();

	m_track->fpos_next_frame = m_savedPositionNextFrame;
	m_track->nextfframe = m_savedNextFrame;
}