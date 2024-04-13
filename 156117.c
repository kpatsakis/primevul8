string_get(unsigned size)
{
gstring * g = store_get(sizeof(gstring) + size);
g->size = size;
g->ptr = 0;
g->s = US(g + 1);
return g;
}