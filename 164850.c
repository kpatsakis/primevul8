void ApplyChannelMatrix::run(Chunk &inChunk, Chunk &outChunk)
{
	switch (m_format)
	{
		case kInt8:
			run<int8_t>(inChunk.buffer, outChunk.buffer, inChunk.frameCount);
			break;
		case kInt16:
			run<int16_t>(inChunk.buffer, outChunk.buffer, inChunk.frameCount);
			break;
		case kInt24:
		case kInt32:
			run<int32_t>(inChunk.buffer, outChunk.buffer, inChunk.frameCount);
			break;
		case kFloat:
			run<float>(inChunk.buffer, outChunk.buffer, inChunk.frameCount);
			break;
		case kDouble:
			run<double>(inChunk.buffer, outChunk.buffer, inChunk.frameCount);
			break;
		default:
			assert(false);
	}
}