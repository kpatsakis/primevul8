stl_check_facets_exact(stl_file *stl) {
  /* This function builds the neighbors list.  No modifications are made
   *  to any of the facets.  The edges are said to match only if all six
   *  floats of the first edge matches all six floats of the second edge.
   */

  stl_hash_edge  edge;
  stl_facet      facet;
  int            i;
  int            j;

  if (stl->error) return;

  stl->stats.connected_edges = 0;
  stl->stats.connected_facets_1_edge = 0;
  stl->stats.connected_facets_2_edge = 0;
  stl->stats.connected_facets_3_edge = 0;

  stl_initialize_facet_check_exact(stl);

  for(i = 0; i < stl->stats.number_of_facets; i++) {
    facet = stl->facet_start[i];

    /* If any two of the three vertices are found to be exactally the same, call them degenerate and remove the facet. */
    if(   !memcmp(&facet.vertex[0], &facet.vertex[1],
                  sizeof(stl_vertex))
          || !memcmp(&facet.vertex[1], &facet.vertex[2],
                     sizeof(stl_vertex))
          || !memcmp(&facet.vertex[0], &facet.vertex[2],
                     sizeof(stl_vertex))) {
      stl->stats.degenerate_facets += 1;
      stl_remove_facet(stl, i);
      i--;
      continue;

    }
    for(j = 0; j < 3; j++) {
      edge.facet_number = i;
      edge.which_edge = j;
      stl_load_edge_exact(stl, &edge, &facet.vertex[j],
                          &facet.vertex[(j + 1) % 3]);

      insert_hash_edge(stl, edge, stl_match_neighbors_exact);
    }
  }
  stl_free_edges(stl);
}