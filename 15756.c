stl_check_facets_nearby(stl_file *stl, float tolerance) {
  stl_hash_edge  edge[3];
  stl_facet      facet;
  int            i;
  int            j;

  if (stl->error) return;

  if(   (stl->stats.connected_facets_1_edge == stl->stats.number_of_facets)
        && (stl->stats.connected_facets_2_edge == stl->stats.number_of_facets)
        && (stl->stats.connected_facets_3_edge == stl->stats.number_of_facets)) {
    /* No need to check any further.  All facets are connected */
    return;
  }

  stl_initialize_facet_check_nearby(stl);

  for(i = 0; i < stl->stats.number_of_facets; i++) {
    facet = stl->facet_start[i];
    for(j = 0; j < 3; j++) {
      if(stl->neighbors_start[i].neighbor[j] == -1) {
        edge[j].facet_number = i;
        edge[j].which_edge = j;
        if(stl_load_edge_nearby(stl, &edge[j], &facet.vertex[j],
                                &facet.vertex[(j + 1) % 3],
                                tolerance)) {
          /* only insert edges that have different keys */
          insert_hash_edge(stl, edge[j], stl_match_neighbors_nearby);
        }
      }
    }
  }

  stl_free_edges(stl);
}