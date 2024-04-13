gstring_reset_unused(gstring * g)
{
store_reset(g->s + (g->size = g->ptr + 1));
}