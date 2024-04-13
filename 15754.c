stl_fill_holes(stl_file *stl) {
  stl_facet facet;
  stl_facet new_facet;
  int neighbors_initial[3];
  stl_hash_edge edge;
  int first_facet;
  int direction;
  int facet_num;
  int vnot;
  int next_edge;
  int pivot_vertex;
  int next_facet;
  int i;
  int j;
  int k;

  if (stl->error) return;

  /* Insert all unconnected edges into hash list */
  stl_initialize_facet_check_nearby(stl);
  for(i = 0; i < stl->stats.number_of_facets; i++) {
    facet = stl->facet_start[i];
    for(j = 0; j < 3; j++) {
      if(stl->neighbors_start[i].neighbor[j] != -1) continue;
      edge.facet_number = i;
      edge.which_edge = j;
      stl_load_edge_exact(stl, &edge, &facet.vertex[j],
                          &facet.vertex[(j + 1) % 3]);

      insert_hash_edge(stl, edge, stl_match_neighbors_exact);
    }
  }

  for(i = 0; i < stl->stats.number_of_facets; i++) {
    facet = stl->facet_start[i];
    neighbors_initial[0] = stl->neighbors_start[i].neighbor[0];
    neighbors_initial[1] = stl->neighbors_start[i].neighbor[1];
    neighbors_initial[2] = stl->neighbors_start[i].neighbor[2];
    first_facet = i;
    for(j = 0; j < 3; j++) {
      if(stl->neighbors_start[i].neighbor[j] != -1) continue;

      new_facet.vertex[0] = facet.vertex[j];
      new_facet.vertex[1] = facet.vertex[(j + 1) % 3];
      if(neighbors_initial[(j + 2) % 3] == -1) {
        direction = 1;
      } else {
        direction = 0;
      }

      facet_num = i;
      vnot = (j + 2) % 3;

      for(;;) {
        if(vnot > 2) {
          if(direction == 0) {
            pivot_vertex = (vnot + 2) % 3;
            next_edge = pivot_vertex;
            direction = 1;
          } else {
            pivot_vertex = (vnot + 1) % 3;
            next_edge = vnot % 3;
            direction = 0;
          }
        } else {
          if(direction == 0) {
            pivot_vertex = (vnot + 1) % 3;
            next_edge = vnot;
          } else {
            pivot_vertex = (vnot + 2) % 3;
            next_edge = pivot_vertex;
          }
        }
        next_facet = stl->neighbors_start[facet_num].neighbor[next_edge];

        if(next_facet == -1) {
          new_facet.vertex[2] = stl->facet_start[facet_num].
                                vertex[vnot % 3];
          stl_add_facet(stl, &new_facet);
          for(k = 0; k < 3; k++) {
            edge.facet_number = stl->stats.number_of_facets - 1;
            edge.which_edge = k;
            stl_load_edge_exact(stl, &edge, &new_facet.vertex[k],
                                &new_facet.vertex[(k + 1) % 3]);

            insert_hash_edge(stl, edge, stl_match_neighbors_exact);
          }
          break;
        } else {
          vnot = stl->neighbors_start[facet_num].
                 which_vertex_not[next_edge];
          facet_num = next_facet;
        }

        if(facet_num == first_facet) {
          /* back to the beginning */
          printf("\
Back to the first facet filling holes: probably a mobius part.\n\
Try using a smaller tolerance or don't do a nearby check\n");
          return;
        }
      }
    }
  }
  free(stl->heads);
  free(stl->tail);
}