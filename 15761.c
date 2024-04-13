stl_which_vertices_to_change(stl_file *stl, stl_hash_edge *edge_a,
                             stl_hash_edge *edge_b, int *facet1, int *vertex1,
                             int *facet2, int *vertex2,
                             stl_vertex *new_vertex1, stl_vertex *new_vertex2) {
  int v1a;			/* pair 1, facet a */
  int v1b;			/* pair 1, facet b */
  int v2a;			/* pair 2, facet a */
  int v2b;			/* pair 2, facet b */

  /* Find first pair */
  if(edge_a->which_edge < 3) {
    v1a = edge_a->which_edge;
    v2a = (edge_a->which_edge + 1) % 3;
  } else {
    v2a = edge_a->which_edge % 3;
    v1a = (edge_a->which_edge + 1) % 3;
  }
  if(edge_b->which_edge < 3) {
    v1b = edge_b->which_edge;
    v2b = (edge_b->which_edge + 1) % 3;
  } else {
    v2b = edge_b->which_edge % 3;
    v1b = (edge_b->which_edge + 1) % 3;
  }

  /* Of the first pair, which vertex, if any, should be changed */
  if(!memcmp(&stl->facet_start[edge_a->facet_number].vertex[v1a],
             &stl->facet_start[edge_b->facet_number].vertex[v1b],
             sizeof(stl_vertex))) {
    /* These facets are already equal.  No need to change. */
    *facet1 = -1;
  } else {
    if(   (stl->neighbors_start[edge_a->facet_number].neighbor[v1a] == -1)
          && (stl->neighbors_start[edge_a->facet_number].
              neighbor[(v1a + 2) % 3] == -1)) {
      /* This vertex has no neighbors.  This is a good one to change */
      *facet1 = edge_a->facet_number;
      *vertex1 = v1a;
      *new_vertex1 = stl->facet_start[edge_b->facet_number].vertex[v1b];
    } else {
      *facet1 = edge_b->facet_number;
      *vertex1 = v1b;
      *new_vertex1 = stl->facet_start[edge_a->facet_number].vertex[v1a];
    }
  }

  /* Of the second pair, which vertex, if any, should be changed */
  if(!memcmp(&stl->facet_start[edge_a->facet_number].vertex[v2a],
             &stl->facet_start[edge_b->facet_number].vertex[v2b],
             sizeof(stl_vertex))) {
    /* These facets are already equal.  No need to change. */
    *facet2 = -1;
  } else {
    if(   (stl->neighbors_start[edge_a->facet_number].neighbor[v2a] == -1)
          && (stl->neighbors_start[edge_a->facet_number].
              neighbor[(v2a + 2) % 3] == -1)) {
      /* This vertex has no neighbors.  This is a good one to change */
      *facet2 = edge_a->facet_number;
      *vertex2 = v2a;
      *new_vertex2 = stl->facet_start[edge_b->facet_number].vertex[v2b];
    } else {
      *facet2 = edge_b->facet_number;
      *vertex2 = v2b;
      *new_vertex2 = stl->facet_start[edge_a->facet_number].vertex[v2a];
    }
  }
}