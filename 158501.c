static BOOL check_response(int *errno_value, int more_errno, uschar *buffer,
  int *yield, uschar **message)
{
*yield = '4';    /* Default setting is to give a temporary error */

/* Handle response timeout */

if (*errno_value == ETIMEDOUT)
  {
  *message = string_sprintf("LMTP timeout after %s", big_buffer);
  if (transport_count > 0)
    *message = string_sprintf("%s (%d bytes written)", *message,
      transport_count);
  *errno_value = 0;
  return FALSE;
  }

/* Handle malformed LMTP response */

if (*errno_value == ERRNO_SMTPFORMAT)
  {
  *message = string_sprintf("Malformed LMTP response after %s: %s",
    big_buffer, string_printing(buffer));
  return FALSE;
  }

/* Handle a failed filter process error; can't send QUIT as we mustn't
end the DATA. */

if (*errno_value == ERRNO_FILTER_FAIL)
  {
  *message = string_sprintf("transport filter process failed (%d)%s",
    more_errno,
    (more_errno == EX_EXECFAILED)? ": unable to execute command" : "");
  return FALSE;
  }

/* Handle a failed add_headers expansion; can't send QUIT as we mustn't
end the DATA. */

if (*errno_value == ERRNO_CHHEADER_FAIL)
  {
  *message =
    string_sprintf("failed to expand headers_add or headers_remove: %s",
      expand_string_message);
  return FALSE;
  }

/* Handle failure to write a complete data block */

if (*errno_value == ERRNO_WRITEINCOMPLETE)
  {
  *message = string_sprintf("failed to write a data block");
  return FALSE;
  }

/* Handle error responses from the remote process. */

if (buffer[0] != 0)
  {
  const uschar *s = string_printing(buffer);
  *message = string_sprintf("LMTP error after %s: %s", big_buffer, s);
  *yield = buffer[0];
  return TRUE;
  }

/* No data was read. If there is no errno, this must be the EOF (i.e.
connection closed) case, which causes deferral. Otherwise, leave the errno
value to be interpreted. In all cases, we have to assume the connection is now
dead. */

if (*errno_value == 0)
  {
  *errno_value = ERRNO_SMTPCLOSED;
  *message = string_sprintf("LMTP connection closed after %s", big_buffer);
  }

return FALSE;
}