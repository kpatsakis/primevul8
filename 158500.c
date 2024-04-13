acl_var_write(uschar *name, uschar *value, void *ctx)
{
FILE *f = (FILE *)ctx;
fprintf(f, "-acl%c %s %d\n%s\n", name[0], name+1, Ustrlen(value), value);
}