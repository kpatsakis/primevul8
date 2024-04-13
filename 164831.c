	void allocate(size_t capacity)
	{
		if (ownsMemory)
			::operator delete(buffer);
		ownsMemory = true;
		buffer = ::operator new(capacity);
	}