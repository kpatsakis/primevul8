is_period_char(unsigned char *ch)
{
    switch (*ch) {
    case ',':
    case '.':
    case ':':
    case ';':
    case '?':
    case '!':
    case ')':
    case ']':
    case '}':
    case '>':
	return 1;
    default:
	return 0;
    }
}