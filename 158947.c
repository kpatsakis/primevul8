static guint lo_key_hash(gconstpointer key)
{
    const struct lo_key *lkey = key;

    return (guint)lkey->ino + (guint)lkey->dev + (guint)lkey->mnt_id;
}