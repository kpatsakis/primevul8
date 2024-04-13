string_from_gstring(gstring * g)
{
if (!g) return NULL;
g->s[g->ptr] = '\0';
return g->s;
}