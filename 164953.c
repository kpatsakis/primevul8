	void convertSignedToUnsigned(const void *src, void *dst, size_t count)
	{
		switch (m_format)
		{
			case kInt8:
				convertSignedToUnsigned<kInt8>(src, dst, count);
				break;
			case kInt16:
				convertSignedToUnsigned<kInt16>(src, dst, count);
				break;
			case kInt24:
				convertSignedToUnsigned<kInt24>(src, dst, count);
				break;
			case kInt32:
				convertSignedToUnsigned<kInt32>(src, dst, count);
				break;
			default:
				assert(false);
		}
	}