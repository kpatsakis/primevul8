void IMA::reset1()
{
	AFframecount nextTrackFrame = m_track->nextfframe;
	m_track->nextfframe = (nextTrackFrame / m_framesPerBlock) *
		m_framesPerBlock;

	m_framesToIgnore = nextTrackFrame - m_track->nextfframe;
	/* postroll = frames2ignore */
}