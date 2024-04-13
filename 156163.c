p_rel_addr(FILE* f, RelAddrType rel_addr, Operation* p, Operation* start)
{
  RelAddrType curr = (RelAddrType )(p - start);

  fprintf(f, "{%d/%d}", rel_addr, curr + rel_addr);
}