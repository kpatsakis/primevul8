node_new_str_crude_char(UChar c)
{
  int i;
  UChar p[1];
  Node* node;

  p[0] = c;
  node = node_new_str_crude(p, p + 1);

  /* clear buf tail */
  for (i = 1; i < NODE_STRING_BUF_SIZE; i++)
    STR_(node)->buf[i] = '\0';

  return node;
}