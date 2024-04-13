stl_match_neighbors_nearby(stl_file *stl,
                           stl_hash_edge *edge_a, stl_hash_edge *edge_b) {
  int facet1;
  int facet2;
  int vertex1;
  int vertex2;
  int vnot1;
  int vnot2;
  stl_vertex new_vertex1;
  stl_vertex new_vertex2;

  if (stl->error) return;

  stl_record_neighbors(stl, edge_a, edge_b);
  stl_which_vertices_to_change(stl, edge_a, edge_b, &facet1, &vertex1,
                               &facet2, &vertex2, &new_vertex1, &new_vertex2);
  if(facet1 != -1) {
    if(facet1 == edge_a->facet_number) {
      vnot1 = (edge_a->which_edge + 2) % 3;
    } else {
      vnot1 = (edge_b->which_edge + 2) % 3;
    }
    if(((vnot1 + 2) % 3) == vertex1) {
      vnot1 += 3;
    }
    stl_change_vertices(stl, facet1, vnot1, new_vertex1);
  }
  if(facet2 != -1) {
    if(facet2 == edge_a->facet_number) {
      vnot2 = (edge_a->which_edge + 2) % 3;
    } else {
      vnot2 = (edge_b->which_edge + 2) % 3;
    }
    if(((vnot2 + 2) % 3) == vertex2) {
      vnot2 += 3;
    }
    stl_change_vertices(stl, facet2, vnot2, new_vertex2);
  }
  stl->stats.edges_fixed += 2;
}