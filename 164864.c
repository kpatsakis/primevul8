size_t _AudioFormat::bytesPerSample() const
{
	return bytesPerSample(!isPacked());
}