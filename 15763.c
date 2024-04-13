stl_remove_facet(stl_file *stl, int facet_number) {
  int neighbor[3];
  int vnot[3];
  int i;
  int j;

  if (stl->error) return;

  stl->stats.facets_removed += 1;
  /* Update list of connected edges */
  j = ((stl->neighbors_start[facet_number].neighbor[0] == -1) +
       (stl->neighbors_start[facet_number].neighbor[1] == -1) +
       (stl->neighbors_start[facet_number].neighbor[2] == -1));
  if(j == 2) {
    stl->stats.connected_facets_1_edge -= 1;
  } else if(j == 1) {
    stl->stats.connected_facets_2_edge -= 1;
    stl->stats.connected_facets_1_edge -= 1;
  } else if(j == 0) {
    stl->stats.connected_facets_3_edge -= 1;
    stl->stats.connected_facets_2_edge -= 1;
    stl->stats.connected_facets_1_edge -= 1;
  }

  stl->facet_start[facet_number] =
    stl->facet_start[stl->stats.number_of_facets - 1];
  /* I could reallocate at this point, but it is not really necessary. */
  stl->neighbors_start[facet_number] =
    stl->neighbors_start[stl->stats.number_of_facets - 1];
  stl->stats.number_of_facets -= 1;

  for(i = 0; i < 3; i++) {
    neighbor[i] = stl->neighbors_start[facet_number].neighbor[i];
    vnot[i] = stl->neighbors_start[facet_number].which_vertex_not[i];
  }

  for(i = 0; i < 3; i++) {
    if(neighbor[i] != -1) {
      if(stl->neighbors_start[neighbor[i]].neighbor[(vnot[i] + 1)% 3] !=
          stl->stats.number_of_facets) {
        printf("\
in stl_remove_facet: neighbor = %d numfacets = %d this is wrong\n",
               stl->neighbors_start[neighbor[i]].neighbor[(vnot[i] + 1)% 3],
               stl->stats.number_of_facets);
        return;
      }
      stl->neighbors_start[neighbor[i]].neighbor[(vnot[i] + 1)% 3]
        = facet_number;
    }
  }
}