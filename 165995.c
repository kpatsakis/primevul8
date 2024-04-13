MatchClass(class, input)
    register char class;
    register char input;
{
    register char c;
    register int retval;
    retval = 0;

    switch (class)
    {
	/* ESCAPE */

    case '?':			/* ?? -> ? */
	if (input == '?')
	{
	    retval = 1;
	}
	break;

	/* ILLOGICAL GROUPINGS (ie: not in ctype.h) */

    case 'V':
    case 'v':			/* vowels */
	c = CRACK_TOLOWER(input);
	if (strchr("aeiou", c))
	{
	    retval = 1;
	}
	break;

    case 'C':
    case 'c':			/* consonants */
	c = CRACK_TOLOWER(input);
	if (strchr("bcdfghjklmnpqrstvwxyz", c))
	{
	    retval = 1;
	}
	break;

    case 'W':
    case 'w':			/* whitespace */
	if (strchr("\t ", input))
	{
	    retval = 1;
	}
	break;

    case 'P':
    case 'p':			/* punctuation */
	if (strchr(".`,:;'!?\"", input))
	{
	    retval = 1;
	}
	break;

    case 'S':
    case 's':			/* symbols */
	if (strchr("$%%^&*()-_+=|\\[]{}#@/~", input))
	{
	    retval = 1;
	}
	break;

	/* LOGICAL GROUPINGS */

    case 'L':
    case 'l':			/* lowercase */
	if (islower(input))
	{
	    retval = 1;
	}
	break;

    case 'U':
    case 'u':			/* uppercase */
	if (isupper(input))
	{
	    retval = 1;
	}
	break;

    case 'A':
    case 'a':			/* alphabetic */
	if (isalpha(input))
	{
	    retval = 1;
	}
	break;

    case 'X':
    case 'x':			/* alphanumeric */
	if (isalnum(input))
	{
	    retval = 1;
	}
	break;

    case 'D':
    case 'd':			/* digits */
	if (isdigit(input))
	{
	    retval = 1;
	}
	break;

    default:
	Debug(1, "MatchClass: unknown class %c\n", class);
	return (0);
	break;
    }

    if (isupper(class))
    {
	return (!retval);
    }
    return (retval);
}