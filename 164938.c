static void linear2alaw_buf (const int16_t *linear, uint8_t *alaw, int nsamples)
{
	for (int i=0; i < nsamples; i++)
		alaw[i] = _af_linear2alaw(linear[i]);
}