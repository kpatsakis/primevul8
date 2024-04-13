	~Chunk()
	{
		if (ownsMemory)
			::operator delete(buffer);
		buffer = NULL;
	}