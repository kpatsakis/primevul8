static ssize_t lo_add_fd_mapping(struct lo_data *lo, int fd)
{
    struct lo_map_elem *elem;

    elem = lo_map_alloc_elem(&lo->fd_map);
    if (!elem) {
        return -1;
    }

    elem->fd = fd;
    return elem - lo->fd_map.elems;
}