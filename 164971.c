	virtual const char *name() const
	{
		return mode() == Compress ? "g711compress" : "g711decompress";
	}