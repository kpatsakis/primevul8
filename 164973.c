static void alaw2linear_buf (const uint8_t *alaw, int16_t *linear, int nsamples)
{
	for (int i=0; i < nsamples; i++)
		linear[i] = _af_alaw2linear(alaw[i]);
}