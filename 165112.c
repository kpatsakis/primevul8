static void ulaw2linear_buf (const uint8_t *ulaw, int16_t *linear, int nsamples)
{
	for (int i=0; i < nsamples; i++)
		linear[i] = _af_ulaw2linear(ulaw[i]);
}