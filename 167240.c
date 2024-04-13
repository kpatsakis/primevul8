const char *getname(const uid_t uid)
{
    static char number[11];

    snprintf(number, sizeof number, "%-10d", uid);
    return number;
}