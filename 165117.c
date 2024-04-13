static int16_t ms_adpcm_decode_sample (ms_adpcm_state *state,
	uint8_t code, const int16_t *coefficient)
{
	const int32_t MAX_INT16 = 32767, MIN_INT16 = -32768;
	const int32_t adaptive[] =
	{
		230, 230, 230, 230, 307, 409, 512, 614,
		768, 614, 512, 409, 307, 230, 230, 230
	};
	int32_t	linearSample, delta;

	linearSample = ((state->sample1 * coefficient[0]) +
		(state->sample2 * coefficient[1])) / 256;

	if (code & 0x08)
		linearSample += state->delta * (code-0x10);
	else
		linearSample += state->delta * code;

	/* Clamp linearSample to a signed 16-bit value. */
	if (linearSample < MIN_INT16)
		linearSample = MIN_INT16;
	else if (linearSample > MAX_INT16)
		linearSample = MAX_INT16;

	delta = ((int32_t) state->delta * adaptive[code])/256;
	if (delta < 16)
	{
		delta = 16;
	}

	state->delta = delta;
	state->sample2 = state->sample1;
	state->sample1 = linearSample;

	/*
		Because of earlier range checking, new_sample will be
		in the range of an int16_t.
	*/
	return (int16_t) linearSample;
}