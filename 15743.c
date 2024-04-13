stl_initialize_facet_check_nearby(stl_file *stl) {
  int i;

  if (stl->error) return;

  stl->stats.malloced = 0;
  stl->stats.freed = 0;
  stl->stats.collisions = 0;

  /*  tolerance = STL_MAX(stl->stats.shortest_edge, tolerance);*/
  /*  tolerance = STL_MAX((stl->stats.bounding_diameter / 500000.0), tolerance);*/
  /*  tolerance *= 0.5;*/

  stl->M = 81397;

  stl->heads = (stl_hash_edge**)calloc(stl->M, sizeof(*stl->heads));
  if(stl->heads == NULL) perror("stl_initialize_facet_check_nearby");

  stl->tail = (stl_hash_edge*)malloc(sizeof(stl_hash_edge));
  if(stl->tail == NULL) perror("stl_initialize_facet_check_nearby");

  stl->tail->next = stl->tail;

  for(i = 0; i < stl->M; i++) {
    stl->heads[i] = stl->tail;
  }
}