debug_print_ids(uschar *s)
{
debug_printf("%s uid=%ld gid=%ld euid=%ld egid=%ld\n", s,
  (long int)getuid(), (long int)getgid(), (long int)geteuid(),
  (long int)getegid());
}