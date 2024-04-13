	SharedPtr &operator =(const SharedPtr &p)
	{
		if (m_ptr != p.m_ptr)
		{
			if (p.m_ptr) p.m_ptr->retain();
			if (m_ptr) m_ptr->release();
			m_ptr = p.m_ptr;
		}
		return *this;
	}