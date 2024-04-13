rb_char_to_option_kcode(int c, int *option, int *kcode)
{
    *option = 0;

    switch (c) {
      case 'n':
        *kcode = rb_ascii8bit_encindex();
        return (*option = ARG_ENCODING_NONE);
      case 'e':
	*kcode = ENCINDEX_EUC_JP;
	break;
      case 's':
	*kcode = ENCINDEX_Windows_31J;
	break;
      case 'u':
	*kcode = rb_utf8_encindex();
	break;
      default:
	*kcode = -1;
	return (*option = char_to_option(c));
    }
    *option = ARG_ENCODING_FIXED;
    return 1;
}