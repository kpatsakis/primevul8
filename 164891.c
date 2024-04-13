size_t _AudioFormat::bytesPerSample(bool stretch3to4) const
{
	return _af_format_sample_size_uncompressed(this, stretch3to4);
}