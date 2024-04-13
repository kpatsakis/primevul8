	SharedPtr(T *ptr) : m_ptr(ptr)
	{
		if (m_ptr) m_ptr->retain();
	}