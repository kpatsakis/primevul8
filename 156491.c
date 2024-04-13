		virtual boost::system::error_condition default_error_condition(
			int ev) const BOOST_SYSTEM_NOEXCEPT
		{ return boost::system::error_condition(ev, *this); }