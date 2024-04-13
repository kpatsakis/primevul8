copy_salt_to_sipkey(XML_Parser parser, struct sipkey *key) {
  key->k[0] = 0;
  key->k[1] = get_hash_secret_salt(parser);
}