stl_match_neighbors_exact(stl_file *stl,
                          stl_hash_edge *edge_a, stl_hash_edge *edge_b) {
  if (stl->error) return;
  stl_record_neighbors(stl, edge_a, edge_b);
}