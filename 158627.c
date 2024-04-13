smtp_message_code(uschar **code, int *codelen, uschar **msg, uschar **log_msg,
  BOOL check_valid)
{
int n;
int ovector[3];

if (!msg || !*msg) return;

if ((n = pcre_exec(regex_smtp_code, NULL, CS *msg, Ustrlen(*msg), 0,
  PCRE_EOPT, ovector, sizeof(ovector)/sizeof(int))) < 0) return;

if (check_valid && (*msg)[0] != (*code)[0])
  {
  log_write(0, LOG_MAIN|LOG_PANIC, "configured error code starts with "
    "incorrect digit (expected %c) in \"%s\"", (*code)[0], *msg);
  if (log_msg != NULL && *log_msg == *msg)
    *log_msg = string_sprintf("%s %s", *code, *log_msg + ovector[1]);
  }
else
  {
  *code = *msg;
  *codelen = ovector[1];    /* Includes final space */
  }
*msg += ovector[1];         /* Chop the code off the message */
return;
}