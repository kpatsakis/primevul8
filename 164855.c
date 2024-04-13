	SharedPtr(const SharedPtr &p) : m_ptr(p.m_ptr)
	{
		if (m_ptr) m_ptr->retain();
	}