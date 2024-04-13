stl_change_vertices(stl_file *stl, int facet_num, int vnot,
                    stl_vertex new_vertex) {
  int first_facet;
  int direction;
  int next_edge;
  int pivot_vertex;

  if (stl->error) return;

  first_facet = facet_num;
  direction = 0;

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
    stl->facet_start[facet_num].vertex[pivot_vertex] = new_vertex;
    vnot = stl->neighbors_start[facet_num].which_vertex_not[next_edge];
    facet_num = stl->neighbors_start[facet_num].neighbor[next_edge];

    if(facet_num == -1) {
      break;
    }

    if(facet_num == first_facet) {
      /* back to the beginning */
      printf("\
Back to the first facet changing vertices: probably a mobius part.\n\
Try using a smaller tolerance or don't do a nearby check\n");
      return;
    }
  }
}