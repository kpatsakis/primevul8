static bool imap_is_bchar(char ch)
{
  switch(ch) {
    /* bchar */
    case ':': case '@': case '/':
    /* bchar -> achar */
    case '&': case '=':
    /* bchar -> achar -> uchar -> unreserved */
    case '0': case '1': case '2': case '3': case '4': case '5': case '6':
    case '7': case '8': case '9':
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
    case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
    case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
    case 'V': case 'W': case 'X': case 'Y': case 'Z':
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
    case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
    case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
    case 'v': case 'w': case 'x': case 'y': case 'z':
    case '-': case '.': case '_': case '~':
    /* bchar -> achar -> uchar -> sub-delims-sh */
    case '!': case '$': case '\'': case '(': case ')': case '*':
    case '+': case ',':
    /* bchar -> achar -> uchar -> pct-encoded */
    case '%': /* HEXDIG chars are already included above */
      return true;

    default:
      return false;
  }
}