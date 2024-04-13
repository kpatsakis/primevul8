stl_add_facet(stl_file *stl, stl_facet *new_facet) {
  if (stl->error) return;

  stl->stats.facets_added += 1;
  if(stl->stats.facets_malloced < stl->stats.number_of_facets + 1) {
    stl->facet_start = (stl_facet*)realloc(stl->facet_start,
                                           (sizeof(stl_facet) * (stl->stats.facets_malloced + 256)));
    if(stl->facet_start == NULL) perror("stl_add_facet");
    stl->neighbors_start = (stl_neighbors*)realloc(stl->neighbors_start,
                           (sizeof(stl_neighbors) * (stl->stats.facets_malloced + 256)));
    if(stl->neighbors_start == NULL) perror("stl_add_facet");
    stl->stats.facets_malloced += 256;
  }
  stl->facet_start[stl->stats.number_of_facets] = *new_facet;

  /* note that the normal vector is not set here, just initialized to 0 */
  stl->facet_start[stl->stats.number_of_facets].normal.x = 0.0;
  stl->facet_start[stl->stats.number_of_facets].normal.y = 0.0;
  stl->facet_start[stl->stats.number_of_facets].normal.z = 0.0;

  stl->neighbors_start[stl->stats.number_of_facets].neighbor[0] = -1;
  stl->neighbors_start[stl->stats.number_of_facets].neighbor[1] = -1;
  stl->neighbors_start[stl->stats.number_of_facets].neighbor[2] = -1;
  stl->stats.number_of_facets += 1;
}