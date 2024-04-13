static void free_only_key(HtPPKv *kv) {
	free (kv->key);
}