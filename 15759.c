stl_record_neighbors(stl_file *stl,
                     stl_hash_edge *edge_a, stl_hash_edge *edge_b) {
  int i;
  int j;

  if (stl->error) return;

  /* Facet a's neighbor is facet b */
  stl->neighbors_start[edge_a->facet_number].neighbor[edge_a->which_edge % 3] =
    edge_b->facet_number;	/* sets the .neighbor part */

  stl->neighbors_start[edge_a->facet_number].
  which_vertex_not[edge_a->which_edge % 3] =
    (edge_b->which_edge + 2) % 3; /* sets the .which_vertex_not part */

  /* Facet b's neighbor is facet a */
  stl->neighbors_start[edge_b->facet_number].neighbor[edge_b->which_edge % 3] =
    edge_a->facet_number;	/* sets the .neighbor part */

  stl->neighbors_start[edge_b->facet_number].
  which_vertex_not[edge_b->which_edge % 3] =
    (edge_a->which_edge + 2) % 3; /* sets the .which_vertex_not part */

  if(   ((edge_a->which_edge < 3) && (edge_b->which_edge < 3))
        || ((edge_a->which_edge > 2) && (edge_b->which_edge > 2))) {
    /* these facets are oriented in opposite directions.  */
    /*  their normals are probably messed up. */
    stl->neighbors_start[edge_a->facet_number].
    which_vertex_not[edge_a->which_edge % 3] += 3;
    stl->neighbors_start[edge_b->facet_number].
    which_vertex_not[edge_b->which_edge % 3] += 3;
  }


  /* Count successful connects */
  /* Total connects */
  stl->stats.connected_edges += 2;
  /* Count individual connects */
  i = ((stl->neighbors_start[edge_a->facet_number].neighbor[0] == -1) +
       (stl->neighbors_start[edge_a->facet_number].neighbor[1] == -1) +
       (stl->neighbors_start[edge_a->facet_number].neighbor[2] == -1));
  j = ((stl->neighbors_start[edge_b->facet_number].neighbor[0] == -1) +
       (stl->neighbors_start[edge_b->facet_number].neighbor[1] == -1) +
       (stl->neighbors_start[edge_b->facet_number].neighbor[2] == -1));
  if(i == 2) {
    stl->stats.connected_facets_1_edge +=1;
  } else if(i == 1) {
    stl->stats.connected_facets_2_edge +=1;
  } else {
    stl->stats.connected_facets_3_edge +=1;
  }
  if(j == 2) {
    stl->stats.connected_facets_1_edge +=1;
  } else if(j == 1) {
    stl->stats.connected_facets_2_edge +=1;
  } else {
    stl->stats.connected_facets_3_edge +=1;
  }
}