static void linear2ulaw_buf (const int16_t *linear, uint8_t *ulaw, int nsamples)
{
	for (int i=0; i < nsamples; i++)
		ulaw[i] = _af_linear2ulaw(linear[i]);
}