othercase(charset,ch)
CHARSET_INFO *charset;
int ch;
{
	/*
	  In MySQL some multi-byte character sets
	  have 'ctype' array but don't have 'to_lower'
	  and 'to_upper' arrays. In this case we handle
	  only basic latin letters a..z and A..Z.
	  
	  If 'to_lower' and 'to_upper' arrays are empty in a character set,
	  then my_isalpha(cs, ch) should never return TRUE for characters
	  other than basic latin letters. Otherwise it should be
	  considered as a mistake in character set definition.
	*/
	assert(my_isalpha(charset,ch));
	if (my_isupper(charset,ch))
	{
		return(charset->to_lower ? my_tolower(charset,ch) :
		                          ch - 'A' + 'a');
	}
	else if (my_islower(charset,ch))
	{
		return(charset->to_upper ? my_toupper(charset,ch) :
		                          ch - 'a' + 'A');
	}
	else			/* peculiar, but could happen */
		return(ch);
}