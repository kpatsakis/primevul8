int MSADPCM::decodeBlock (const uint8_t *encoded, int16_t *decoded)
{
	int16_t *coefficient[2];
	ms_adpcm_state decoderState[2];
	ms_adpcm_state *state[2];

	int channelCount = m_track->f.channelCount;

	/* Calculate the number of bytes needed for decoded data. */
	int outputLength = m_framesPerBlock * sizeof (int16_t) * channelCount;

	state[0] = &decoderState[0];
	if (channelCount == 2)
		state[1] = &decoderState[1];
	else
		state[1] = &decoderState[0];

	/* Initialize predictor. */
	for (int i=0; i<channelCount; i++)
	{
		state[i]->predictor = *encoded++;
		assert(state[i]->predictor < m_numCoefficients);
	}

	/* Initialize delta. */
	for (int i=0; i<channelCount; i++)
	{
		state[i]->delta = (encoded[1]<<8) | encoded[0];
		encoded += sizeof (uint16_t);
	}

	/* Initialize first two samples. */
	for (int i=0; i<channelCount; i++)
	{
		state[i]->sample1 = (encoded[1]<<8) | encoded[0];
		encoded += sizeof (uint16_t);
	}

	for (int i=0; i<channelCount; i++)
	{
		state[i]->sample2 = (encoded[1]<<8) | encoded[0];
		encoded += sizeof (uint16_t);
	}

	coefficient[0] = m_coefficients[state[0]->predictor];
	coefficient[1] = m_coefficients[state[1]->predictor];

	for (int i=0; i<channelCount; i++)
		*decoded++ = state[i]->sample2;

	for (int i=0; i<channelCount; i++)
		*decoded++ = state[i]->sample1;

	/*
		The first two samples have already been 'decoded' in
		the block header.
	*/
	int samplesRemaining = (m_framesPerBlock - 2) * m_track->f.channelCount;

	while (samplesRemaining > 0)
	{
		uint8_t code;
		int16_t newSample;

		code = *encoded >> 4;
		newSample = ms_adpcm_decode_sample(state[0], code, coefficient[0]);
		*decoded++ = newSample;

		code = *encoded & 0x0f;
		newSample = ms_adpcm_decode_sample(state[1], code, coefficient[1]);
		*decoded++ = newSample;

		encoded++;
		samplesRemaining -= 2;
	}

	return outputLength;
}