stl_compare_function(stl_hash_edge *edge_a, stl_hash_edge *edge_b) {
  if(edge_a->facet_number == edge_b->facet_number) {
    return 1;			/* Don't match edges of the same facet */
  } else {
    return memcmp(edge_a, edge_b, SIZEOF_EDGE_SORT);
  }
}