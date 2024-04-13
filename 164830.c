static FormatCode getFormatCode(const _AudioFormat &format)
{
	if (format.sampleFormat == AF_SAMPFMT_FLOAT)
		return kFloat;
	if (format.sampleFormat == AF_SAMPFMT_DOUBLE)
		return kDouble;
	if (format.isInteger())
	{
		switch (format.bytesPerSample(false))
		{
			case 1: return kInt8;
			case 2: return kInt16;
			case 3: return kInt24;
			case 4: return kInt32;
		}
	}

	/* NOTREACHED */
	assert(false);
	return kUndefined;
}