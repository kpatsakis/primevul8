static int parse_number(char *string, STRLEN len, char **end)
{
    int seen_neg;
    int seen_dec;
    int seen_e;
    int seen_plus;
    int seen_digit;
    char *cp;

    seen_neg= seen_dec= seen_e= seen_plus= seen_digit= 0;

    if (len <= 0) {
        len= strlen(string);
    }

    cp= string;

    /* Skip leading whitespace */
    while (*cp && isspace(*cp))
      cp++;

    for ( ; *cp; cp++)
    {
      if ('-' == *cp)
      {
        if (seen_neg >= 2)
        {
          /*
            third '-'. number can contains two '-'.
            because -1e-10 is valid number */
          break;
        }
        seen_neg += 1;
      }
      else if ('.' == *cp)
      {
        if (seen_dec)
        {
          /* second '.' */
          break;
        }
        seen_dec= 1;
      }
      else if ('e' == *cp)
      {
        if (seen_e)
        {
          /* second 'e' */
          break;
        }
        seen_e= 1;
      }
      else if ('+' == *cp)
      {
        if (seen_plus)
        {
          /* second '+' */
          break;
        }
        seen_plus= 1;
      }
      else if (!isdigit(*cp))
      {
        /* Not sure why this was changed */
        /* seen_digit= 1; */
        break;
      }
    }

    *end= cp;

    /* length 0 -> not a number */
    /* Need to revisit this */
    /*if (len == 0 || cp - string < (int) len || seen_digit == 0) {*/
    if (len == 0 || cp - string < (int) len) {
        return -1;
    }

    return 0;
}