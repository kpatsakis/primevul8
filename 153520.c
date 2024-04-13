is_beginning_char(unsigned char *ch)
{
    switch (*ch) {
    case '(':
    case '[':
    case '{':
    case '`':
    case '<':
	return 1;
    default:
	return 0;
    }
}