	void convertUnsignedToSigned(const void *src, void *dst, size_t count)
	{
		switch (m_format)
		{
			case kInt8:
				convertUnsignedToSigned<kInt8>(src, dst, count);
				break;
			case kInt16:
				convertUnsignedToSigned<kInt16>(src, dst, count);
				break;
			case kInt24:
				convertUnsignedToSigned<kInt24>(src, dst, count);
				break;
			case kInt32:
				convertUnsignedToSigned<kInt32>(src, dst, count);
				break;
			default:
				assert(false);
		}
	}