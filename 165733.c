count_params(imp_xxh_t *imp_xxh, pTHX_ char *statement, bool bind_comment_placeholders)
{
  bool comment_end= false;
  char* ptr= statement;
  int num_params= 0;
  int comment_length= 0;
  char c;

  if (DBIc_DBISTATE(imp_xxh)->debug >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh), ">count_params statement %s\n", statement);

  while ( (c = *ptr++) )
  {
    switch (c) {
      /* so, this is a -- comment, so let's burn up characters */
    case '-':
      {
          if (bind_comment_placeholders)
          {
              c = *ptr++;
              break;
          }
          else
          {
              comment_length= 1;
              /* let's see if the next one is a dash */
              c = *ptr++;

              if  (c == '-') {
                  /* if two dashes, ignore everything until newline */
                  while ((c = *ptr))
                  {
                      if (DBIc_DBISTATE(imp_xxh)->debug >= 2)
                          PerlIO_printf(DBIc_LOGPIO(imp_xxh), "%c\n", c);
                      ptr++;
                      comment_length++;
                      if (c == '\n')
                      {
                          comment_end= true;
                          break;
                      }
                  }
                  /*
                    if not comment_end, the comment never ended and we need to iterate
                    back to the beginning of where we started and let the database 
                    handle whatever is in the statement
                */
                  if (! comment_end)
                      ptr-= comment_length;
              }
              /* otherwise, only one dash/hyphen, backtrack by one */
              else
                  ptr--;
              break;
          }
      }
    /* c-type comments */
    case '/':
      {
          if (bind_comment_placeholders)
          {
              c = *ptr++;
              break;
          }
          else
          {
              c = *ptr++;
              /* let's check if the next one is an asterisk */
              if  (c == '*')
              {
                  comment_length= 0;
                  comment_end= false;
                  /* ignore everything until closing comment */
                  while ((c= *ptr))
                  {
                      ptr++;
                      comment_length++;

                      if (c == '*')
                      {
                          c = *ptr++;
                          /* alas, end of comment */
                          if (c == '/')
                          {
                              comment_end= true;
                              break;
                          }
                          /*
                            nope, just an asterisk, not so fast, not
                            end of comment, go back one
                        */
                          else
                              ptr--;
                      }
                  }
                  /*
                    if the end of the comment was never found, we have
                    to backtrack to wherever we first started skipping
                    over the possible comment.
                    This means we will pass the statement to the database
                    to see its own fate and issue the error
                */
                  if (!comment_end)
                      ptr -= comment_length;
              }
              else
                  ptr--;
              break;
          }
      }
    case '`':
    case '"':
    case '\'':
      /* Skip string */
      {
        char end_token = c;
        while ((c = *ptr)  &&  c != end_token)
        {
          if (c == '\\')
            if (! *(++ptr))
              continue;

          ++ptr;
        }
        if (c)
          ++ptr;
        break;
      }

    case '?':
      ++num_params;
      break;

    default:
      break;
    }
  }
  return num_params;
}