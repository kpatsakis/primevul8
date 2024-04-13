expansion_test_line(uschar * line)
{
int len;
BOOL dummy_macexp;

Ustrncpy(big_buffer, line, big_buffer_size);
big_buffer[big_buffer_size-1] = '\0';
len = Ustrlen(big_buffer);

(void) macros_expand(0, &len, &dummy_macexp);

if (isupper(big_buffer[0]))
  {
  if (macro_read_assignment(big_buffer))
    printf("Defined macro '%s'\n", mlast->name);
  }
else
  if ((line = expand_string(big_buffer))) printf("%s\n", CS line);
  else printf("Failed: %s\n", expand_string_message);
}