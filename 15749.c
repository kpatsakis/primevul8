stl_free_edges(stl_file *stl) {
  int i;
  stl_hash_edge *temp;

  if (stl->error) return;

  if(stl->stats.malloced != stl->stats.freed) {
    for(i = 0; i < stl->M; i++) {
      for(temp = stl->heads[i]; stl->heads[i] != stl->tail;
          temp = stl->heads[i]) {
        stl->heads[i] = stl->heads[i]->next;
        free(temp);
        stl->stats.freed++;
      }
    }
  }
  free(stl->heads);
  free(stl->tail);
}