	SharedPtr &operator =(T *ptr)
	{
		if (m_ptr != ptr)
		{
			if (ptr) ptr->retain();
			if (m_ptr) m_ptr->release();
			m_ptr = ptr;
		}
		return *this;
	}