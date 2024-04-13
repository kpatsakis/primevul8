void G711::reset2()
{
	int framesize = m_inChunk->f.channelCount;

	m_track->fpos_next_frame = m_track->fpos_first_frame +
		framesize * m_track->nextfframe;

	m_track->frames2ignore = 0;
}