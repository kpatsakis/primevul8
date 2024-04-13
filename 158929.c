static gboolean lo_key_equal(gconstpointer a, gconstpointer b)
{
    const struct lo_key *la = a;
    const struct lo_key *lb = b;

    return la->ino == lb->ino && la->dev == lb->dev && la->mnt_id == lb->mnt_id;
}