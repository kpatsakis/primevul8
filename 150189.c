
public void
convert_libmagic_pattern(zval *pattern, int options)
{
		int i, j=0;
		char *t;

		t = (char *) safe_emalloc(Z_STRLEN_P(pattern), 2, 5);
		
		t[j++] = '~';
		
		for (i=0; i<Z_STRLEN_P(pattern); i++, j++) {
			switch (Z_STRVAL_P(pattern)[i]) {
				case '~':
					t[j++] = '\\';
					t[j] = '~';
					break;
				default:
					t[j] = Z_STRVAL_P(pattern)[i];
					break;
			}
		}
		t[j++] = '~';
	
		if (options & PCRE_CASELESS) 
			t[j++] = 'i';
	
		if (options & PCRE_MULTILINE)
			t[j++] = 'm';

		t[j]='\0';
	
		Z_STRVAL_P(pattern) = t;
		Z_STRLEN_P(pattern) = j;