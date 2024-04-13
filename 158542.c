check_never_users(uid_t uid, uid_t *nusers)
{
int i;
if (!nusers) return FALSE;
for (i = 1; i <= (int)(nusers[0]); i++) if (nusers[i] == uid) return TRUE;
return FALSE;
}