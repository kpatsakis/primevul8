compare_gps(const void *_p, const void *_q)
{
    const struct glob_path_stat *p = (const struct glob_path_stat *)_p;
    const struct glob_path_stat *q = (const struct glob_path_stat *)_q;

    return strcmp(p->gps_path, q->gps_path);
}