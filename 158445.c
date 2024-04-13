acl_check_wargs(int where, address_item *addr, const uschar *s,
  uschar **user_msgptr, uschar **log_msgptr)
{
uschar * tmp;
uschar * tmp_arg[9];	/* must match acl_arg[] */
uschar * sav_arg[9];	/* must match acl_arg[] */
int sav_narg;
uschar * name;
int i;
int ret;

if (!(tmp = string_dequote(&s)) || !(name = expand_string(tmp)))
  goto bad;

for (i = 0; i < 9; i++)
  {
  while (*s && isspace(*s)) s++;
  if (!*s) break;
  if (!(tmp = string_dequote(&s)) || !(tmp_arg[i] = expand_string(tmp)))
    {
    tmp = name;
    goto bad;
    }
  }

sav_narg = acl_narg;
acl_narg = i;
for (i = 0; i < acl_narg; i++)
  {
  sav_arg[i] = acl_arg[i];
  acl_arg[i] = tmp_arg[i];
  }
while (i < 9)
  {
  sav_arg[i] = acl_arg[i];
  acl_arg[i++] = NULL;
  }

acl_level++;
ret = acl_check_internal(where, addr, name, user_msgptr, log_msgptr);
acl_level--;

acl_narg = sav_narg;
for (i = 0; i < 9; i++) acl_arg[i] = sav_arg[i];
return ret;

bad:
if (f.expand_string_forcedfail) return ERROR;
*log_msgptr = string_sprintf("failed to expand ACL string \"%s\": %s",
  tmp, expand_string_message);
return f.search_find_defer ? DEFER : ERROR;
}