psf_log_printf (SF_PRIVATE *psf, const char *format, ...)
{	va_list		ap ;
	uint32_t	u ;
	int			d, tens, shift, width, width_specifier, left_align, slen ;
	char		c, *strptr, istr [5], lead_char, sign_char ;

	va_start (ap, format) ;

	while ((c = *format++))
	{	if (c != '%')
		{	log_putchar (psf, c) ;
			continue ;
			} ;

		if (format [0] == '%') /* Handle %% */
		{ 	log_putchar (psf, '%') ;
			format ++ ;
			continue ;
			} ;

		sign_char = 0 ;
		left_align = SF_FALSE ;
		while (1)
		{	switch (format [0])
			{	case ' ' :
				case '+' :
					sign_char = format [0] ;
					format ++ ;
					continue ;

				case '-' :
					left_align = SF_TRUE ;
					format ++ ;
					continue ;

				default : break ;
				} ;

			break ;
			} ;

		if (format [0] == 0)
			break ;

		lead_char = ' ' ;
		if (format [0] == '0')
			lead_char = '0' ;

		width_specifier = 0 ;
		while ((c = *format++) && isdigit (c))
			width_specifier = width_specifier * 10 + (c - '0') ;

		switch (c)
		{	case 0 : /* NULL character. */
					va_end (ap) ;
					return ;

			case 's': /* string */
					strptr = va_arg (ap, char *) ;
					if (strptr == NULL)
						break ;
					slen = strlen (strptr) ;
					width_specifier = width_specifier >= slen ? width_specifier - slen : 0 ;
					if (left_align == SF_FALSE)
						while (width_specifier -- > 0)
							log_putchar (psf, ' ') ;
					while (*strptr)
						log_putchar (psf, *strptr++) ;
					while (width_specifier -- > 0)
						log_putchar (psf, ' ') ;
					break ;

			case 'd': /* int */
					d = va_arg (ap, int) ;

					if (d < 0)
					{	d = -d ;
						sign_char = '-' ;
						if (lead_char != '0' && left_align == SF_FALSE)
							width_specifier -- ;
						} ;

					tens = 1 ;
					width = 1 ;
					while (d / tens >= 10)
					{	tens *= 10 ;
						width ++ ;
						} ;

					width_specifier -= width ;

					if (sign_char == ' ')
					{	log_putchar (psf, ' ') ;
						width_specifier -- ;
						} ;

					if (left_align == SF_FALSE && lead_char != '0')
					{	if (sign_char == '+')
							width_specifier -- ;

						while (width_specifier -- > 0)
							log_putchar (psf, lead_char) ;
						} ;

					if (sign_char == '+' || sign_char == '-')
					{	log_putchar (psf, sign_char) ;
						width_specifier -- ;
						} ;

					if (left_align == SF_FALSE)
						while (width_specifier -- > 0)
							log_putchar (psf, lead_char) ;

					while (tens > 0)
					{	log_putchar (psf, '0' + d / tens) ;
						d %= tens ;
						tens /= 10 ;
						} ;

					while (width_specifier -- > 0)
						log_putchar (psf, lead_char) ;
					break ;

			case 'D': /* sf_count_t */
					{	sf_count_t		D, Tens ;

						D = va_arg (ap, sf_count_t) ;

						if (D == 0)
						{	while (-- width_specifier > 0)
								log_putchar (psf, lead_char) ;
							log_putchar (psf, '0') ;
							break ;
							}
						if (D < 0)
						{	log_putchar (psf, '-') ;
							D = -D ;
							} ;
						Tens = 1 ;
						width = 1 ;
						while (D / Tens >= 10)
						{	Tens *= 10 ;
							width ++ ;
							} ;

						while (width_specifier > width)
						{	log_putchar (psf, lead_char) ;
							width_specifier-- ;
							} ;

						while (Tens > 0)
						{	log_putchar (psf, '0' + D / Tens) ;
							D %= Tens ;
							Tens /= 10 ;
							} ;
						} ;
					break ;

			case 'u': /* unsigned int */
					u = va_arg (ap, unsigned int) ;

					tens = 1 ;
					width = 1 ;
					while (u / tens >= 10)
					{	tens *= 10 ;
						width ++ ;
						} ;

					width_specifier -= width ;

					if (sign_char == ' ')
					{	log_putchar (psf, ' ') ;
						width_specifier -- ;
						} ;

					if (left_align == SF_FALSE && lead_char != '0')
					{	if (sign_char == '+')
							width_specifier -- ;

						while (width_specifier -- > 0)
							log_putchar (psf, lead_char) ;
						} ;

					if (sign_char == '+' || sign_char == '-')
					{	log_putchar (psf, sign_char) ;
						width_specifier -- ;
						} ;

					if (left_align == SF_FALSE)
						while (width_specifier -- > 0)
							log_putchar (psf, lead_char) ;

					while (tens > 0)
					{	log_putchar (psf, '0' + u / tens) ;
						u %= tens ;
						tens /= 10 ;
						} ;

					while (width_specifier -- > 0)
						log_putchar (psf, lead_char) ;
					break ;

			case 'c': /* char */
					c = va_arg (ap, int) & 0xFF ;
					log_putchar (psf, c) ;
					break ;

			case 'x': /* hex */
			case 'X': /* hex */
					d = va_arg (ap, int) ;

					if (d == 0)
					{	while (--width_specifier > 0)
							log_putchar (psf, lead_char) ;
						log_putchar (psf, '0') ;
						break ;
						} ;
					shift = 28 ;
					width = (width_specifier < 8) ? 8 : width_specifier ;
					while (! ((((uint32_t) 0xF) << shift) & d))
					{	shift -= 4 ;
						width -- ;
						} ;

					while (width > 0 && width_specifier > width)
					{	log_putchar (psf, lead_char) ;
						width_specifier-- ;
						} ;

					while (shift >= 0)
					{	c = (d >> shift) & 0xF ;
						log_putchar (psf, (c > 9) ? c + 'A' - 10 : c + '0') ;
						shift -= 4 ;
						} ;
					break ;

			case 'M': /* int2str */
					d = va_arg (ap, int) ;
					if (CPU_IS_LITTLE_ENDIAN)
					{	istr [0] = d & 0xFF ;
						istr [1] = (d >> 8) & 0xFF ;
						istr [2] = (d >> 16) & 0xFF ;
						istr [3] = (d >> 24) & 0xFF ;
						}
					else
					{	istr [3] = d & 0xFF ;
						istr [2] = (d >> 8) & 0xFF ;
						istr [1] = (d >> 16) & 0xFF ;
						istr [0] = (d >> 24) & 0xFF ;
						} ;
					istr [4] = 0 ;
					strptr = istr ;
					while (*strptr)
					{	c = *strptr++ ;
						log_putchar (psf, c) ;
						} ;
					break ;

			default :
					log_putchar (psf, '*') ;
					log_putchar (psf, c) ;
					log_putchar (psf, '*') ;
					break ;
			} /* switch */
		} /* while */

	va_end (ap) ;
	return ;
} /* psf_log_printf */