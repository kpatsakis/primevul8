int IMA::decodeBlock (const uint8_t *encoded, int16_t *decoded)
{
	int channelCount = m_track->f.channelCount;
	adpcm_state state[channelCount];

	for (int c=0; c<channelCount; c++)
	{
		state[c].valprev = (encoded[1]<<8) | encoded[0];
		if (encoded[1] & 0x80)
			state[c].valprev -= 0x10000;

		state[c].index = encoded[2];

		*decoded++ = state[c].valprev;

		encoded += 4;
	}

	_af_adpcm_decoder(encoded, decoded, m_framesPerBlock - 1, channelCount, state);

	return m_framesPerBlock * channelCount * sizeof (int16_t);
}