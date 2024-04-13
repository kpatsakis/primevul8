bool _AudioFormat::isInteger() const
{
	return sampleFormat == AF_SAMPFMT_TWOSCOMP ||
		sampleFormat == AF_SAMPFMT_UNSIGNED;
}