	void release() { if (--m_refCount == 0) delete static_cast<T *>(this); }