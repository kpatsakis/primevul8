const char *getgroup(const gid_t gid)
{
    static char number[11];

    snprintf(number, sizeof number, "%-10d", gid);
    return number;
}