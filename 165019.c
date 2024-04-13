	~SharedPtr()
	{
		if (T *p = m_ptr) p->release();
	}