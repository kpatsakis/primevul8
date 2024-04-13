stl_remove_unconnected_facets(stl_file *stl) {
  /* A couple of things need to be done here.  One is to remove any */
  /* completely unconnected facets (0 edges connected) since these are */
  /* useless and could be completely wrong.   The second thing that needs to */
  /* be done is to remove any degenerate facets that were created during */
  /* stl_check_facets_nearby(). */

  int i;

  if (stl->error) return;

  /* remove degenerate facets */
  for(i = 0; i < stl->stats.number_of_facets; i++) {
    if(   !memcmp(&stl->facet_start[i].vertex[0],
                  &stl->facet_start[i].vertex[1], sizeof(stl_vertex))
          || !memcmp(&stl->facet_start[i].vertex[1],
                     &stl->facet_start[i].vertex[2], sizeof(stl_vertex))
          || !memcmp(&stl->facet_start[i].vertex[0],
                     &stl->facet_start[i].vertex[2], sizeof(stl_vertex))) {
      stl_remove_degenerate(stl, i);
      i--;
    }
  }

  if(stl->stats.connected_facets_1_edge < stl->stats.number_of_facets) {
    /* remove completely unconnected facets */
    for(i = 0; i < stl->stats.number_of_facets; i++) {
      if(   (stl->neighbors_start[i].neighbor[0] == -1)
            && (stl->neighbors_start[i].neighbor[1] == -1)
            && (stl->neighbors_start[i].neighbor[2] == -1)) {
        /* This facet is completely unconnected.  Remove it. */
        stl_remove_facet(stl, i);
        i--;
      }
    }
  }
}