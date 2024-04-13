void PCM::sync1()
{
	m_saved_fpos_next_frame = m_track->fpos_next_frame;
	m_saved_nextfframe = m_track->nextfframe;
}