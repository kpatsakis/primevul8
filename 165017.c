bool _AudioFormat::isFloat() const
{
	return sampleFormat == AF_SAMPFMT_FLOAT ||
		sampleFormat == AF_SAMPFMT_DOUBLE;
}