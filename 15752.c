stl_initialize_facet_check_exact(stl_file *stl) {
  int i;

  if (stl->error) return;

  stl->stats.malloced = 0;
  stl->stats.freed = 0;
  stl->stats.collisions = 0;


  stl->M = 81397;

  for(i = 0; i < stl->stats.number_of_facets ; i++) {
    /* initialize neighbors list to -1 to mark unconnected edges */
    stl->neighbors_start[i].neighbor[0] = -1;
    stl->neighbors_start[i].neighbor[1] = -1;
    stl->neighbors_start[i].neighbor[2] = -1;
  }

  stl->heads = (stl_hash_edge**)calloc(stl->M, sizeof(*stl->heads));
  if(stl->heads == NULL) perror("stl_initialize_facet_check_exact");

  stl->tail = (stl_hash_edge*)malloc(sizeof(stl_hash_edge));
  if(stl->tail == NULL) perror("stl_initialize_facet_check_exact");

  stl->tail->next = stl->tail;

  for(i = 0; i < stl->M; i++) {
    stl->heads[i] = stl->tail;
  }
}