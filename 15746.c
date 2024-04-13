insert_hash_edge(stl_file *stl, stl_hash_edge edge,
                 void (*match_neighbors)(stl_file *stl,
                     stl_hash_edge *edge_a, stl_hash_edge *edge_b)) {
  stl_hash_edge *link;
  stl_hash_edge *new_edge;
  stl_hash_edge *temp;
  int            chain_number;

  if (stl->error) return;

  chain_number = stl_get_hash_for_edge(stl->M, &edge);

  link = stl->heads[chain_number];

  if(link == stl->tail) {
    /* This list doesn't have any edges currently in it.  Add this one. */
    new_edge = (stl_hash_edge*)malloc(sizeof(stl_hash_edge));
    if(new_edge == NULL) perror("insert_hash_edge");
    stl->stats.malloced++;
    *new_edge = edge;
    new_edge->next = stl->tail;
    stl->heads[chain_number] = new_edge;
    return;
  } else  if(!stl_compare_function(&edge, link)) {
    /* This is a match.  Record result in neighbors list. */
    match_neighbors(stl, &edge, link);
    /* Delete the matched edge from the list. */
    stl->heads[chain_number] = link->next;
    free(link);
    stl->stats.freed++;
    return;
  } else {
    /* Continue through the rest of the list */
    for(;;) {
      if(link->next == stl->tail) {
        /* This is the last item in the list. Insert a new edge. */
        new_edge = (stl_hash_edge*)malloc(sizeof(stl_hash_edge));
        if(new_edge == NULL) perror("insert_hash_edge");
        stl->stats.malloced++;
        *new_edge = edge;
        new_edge->next = stl->tail;
        link->next = new_edge;
        stl->stats.collisions++;
        return;
      } else  if(!stl_compare_function(&edge, link->next)) {
        /* This is a match.  Record result in neighbors list. */
        match_neighbors(stl, &edge, link->next);

        /* Delete the matched edge from the list. */
        temp = link->next;
        link->next = link->next->next;
        free(temp);
        stl->stats.freed++;
        return;
      } else {
        /* This is not a match.  Go to the next link */
        link = link->next;
        stl->stats.collisions++;
      }
    }
  }
}