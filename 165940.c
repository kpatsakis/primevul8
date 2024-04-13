str_of_endianness (int end)
{	switch (end)
	{	CASE_NAME (SF_ENDIAN_BIG) ;
		CASE_NAME (SF_ENDIAN_LITTLE) ;
		CASE_NAME (SF_ENDIAN_CPU) ;
		default :
			break ;
		} ;

	/* Zero length string for SF_ENDIAN_FILE. */
	return "" ;
} /* str_of_endianness */