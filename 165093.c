size_t _AudioFormat::bytesPerFrame() const
{
	return bytesPerFrame(!isPacked());
}