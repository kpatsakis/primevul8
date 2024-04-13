float _af_format_sample_size (const _AudioFormat *fmt, bool stretch3to4)
{
	int	compressionIndex;
	float	squishFactor;

	compressionIndex = _af_compression_index_from_id(fmt->compressionType);
	squishFactor = _af_compression[compressionIndex].squishFactor;

	return _af_format_sample_size_uncompressed(fmt, stretch3to4) /
		squishFactor;
}