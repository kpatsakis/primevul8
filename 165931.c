str_of_open_mode (int mode)
{	switch (mode)
	{	CASE_NAME (SFM_READ) ;
		CASE_NAME (SFM_WRITE) ;
		CASE_NAME (SFM_RDWR) ;

		default :
			break ;
		} ;

	return "BAD_MODE" ;
} /* str_of_open_mode */