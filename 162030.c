HandleARDAuth(rfbClient *client)
{
  uint8_t gen[2], len[2];
  size_t keylen;
  uint8_t *mod = NULL, *resp, *pub, *key, *shared;
  gcry_mpi_t genmpi = NULL, modmpi = NULL, respmpi = NULL;
  gcry_mpi_t privmpi = NULL, pubmpi = NULL, keympi = NULL;
  gcry_md_hd_t md5 = NULL;
  gcry_cipher_hd_t aes = NULL;
  gcry_error_t error;
  uint8_t userpass[128], ciphertext[128];
  int passwordLen, usernameLen;
  rfbCredential *cred = NULL;
  rfbBool result = FALSE;

  while (1)
  {
    if (!ReadFromRFBServer(client, (char *)gen, 2))
      break;
    if (!ReadFromRFBServer(client, (char *)len, 2))
      break;

    if (!client->GetCredential)
    {
      rfbClientLog("GetCredential callback is not set.\n");
      break;
    }
    cred = client->GetCredential(client, rfbCredentialTypeUser);
    if (!cred)
    {
      rfbClientLog("Reading credential failed\n");
      break;
    }

    keylen = 256*len[0]+len[1];
    mod = (uint8_t*)malloc(keylen*4);
    if (!mod)
    {
      rfbClientLog("malloc out of memory\n");
      break;
    }
    resp = mod+keylen;
    pub = resp+keylen;
    key = pub+keylen;

    if (!ReadFromRFBServer(client, (char *)mod, keylen))
      break;
    if (!ReadFromRFBServer(client, (char *)resp, keylen))
      break;

    error = gcry_mpi_scan(&genmpi, GCRYMPI_FMT_USG, gen, 2, NULL);
    if (gcry_err_code(error) != GPG_ERR_NO_ERROR)
    {
      rfbClientLog("gcry_mpi_scan error: %s\n", gcry_strerror(error));
      break;
    }
    error = gcry_mpi_scan(&modmpi, GCRYMPI_FMT_USG, mod, keylen, NULL);
    if (gcry_err_code(error) != GPG_ERR_NO_ERROR)
    {
      rfbClientLog("gcry_mpi_scan error: %s\n", gcry_strerror(error));
      break;
    }
    error = gcry_mpi_scan(&respmpi, GCRYMPI_FMT_USG, resp, keylen, NULL);
    if (gcry_err_code(error) != GPG_ERR_NO_ERROR)
    {
      rfbClientLog("gcry_mpi_scan error: %s\n", gcry_strerror(error));
      break;
    }

    privmpi = gcry_mpi_new(keylen);
    if (!privmpi)
    {
      rfbClientLog("gcry_mpi_new out of memory\n");
      break;
    }
    gcry_mpi_randomize(privmpi, (keylen/8)*8, GCRY_STRONG_RANDOM);

    pubmpi = gcry_mpi_new(keylen);
    if (!pubmpi)
    {
      rfbClientLog("gcry_mpi_new out of memory\n");
      break;
    }
    gcry_mpi_powm(pubmpi, genmpi, privmpi, modmpi);

    keympi = gcry_mpi_new(keylen);
    if (!keympi)
    {
      rfbClientLog("gcry_mpi_new out of memory\n");
      break;
    }
    gcry_mpi_powm(keympi, respmpi, privmpi, modmpi);

    if (!rfbMpiToBytes(pubmpi, pub, keylen))
      break;
    if (!rfbMpiToBytes(keympi, key, keylen))
      break;

    error = gcry_md_open(&md5, GCRY_MD_MD5, 0);
    if (gcry_err_code(error) != GPG_ERR_NO_ERROR)
    {
      rfbClientLog("gcry_md_open error: %s\n", gcry_strerror(error));
      break;
    }
    gcry_md_write(md5, key, keylen);
    error = gcry_md_final(md5);
    if (gcry_err_code(error) != GPG_ERR_NO_ERROR)
    {
      rfbClientLog("gcry_md_final error: %s\n", gcry_strerror(error));
      break;
    }
    shared = gcry_md_read(md5, GCRY_MD_MD5);

    passwordLen = strlen(cred->userCredential.password)+1;
    usernameLen = strlen(cred->userCredential.username)+1;
    if (passwordLen > sizeof(userpass)/2)
      passwordLen = sizeof(userpass)/2;
    if (usernameLen > sizeof(userpass)/2)
      usernameLen = sizeof(userpass)/2;

    gcry_randomize(userpass, sizeof(userpass), GCRY_STRONG_RANDOM);
    memcpy(userpass, cred->userCredential.username, usernameLen);
    memcpy(userpass+sizeof(userpass)/2, cred->userCredential.password, passwordLen);

    error = gcry_cipher_open(&aes, GCRY_CIPHER_AES128, GCRY_CIPHER_MODE_ECB, 0);
    if (gcry_err_code(error) != GPG_ERR_NO_ERROR)
    {
      rfbClientLog("gcry_cipher_open error: %s\n", gcry_strerror(error));
      break;
    }
    error = gcry_cipher_setkey(aes, shared, 16);
    if (gcry_err_code(error) != GPG_ERR_NO_ERROR)
    {
      rfbClientLog("gcry_cipher_setkey error: %s\n", gcry_strerror(error));
      break;
    }
    error = gcry_cipher_encrypt(aes, ciphertext, sizeof(ciphertext), userpass, sizeof(userpass));
    if (gcry_err_code(error) != GPG_ERR_NO_ERROR)
    {
      rfbClientLog("gcry_cipher_encrypt error: %s\n", gcry_strerror(error));
      break;
    }

    if (!WriteToRFBServer(client, (char *)ciphertext, sizeof(ciphertext)))
      break;
    if (!WriteToRFBServer(client, (char *)pub, keylen))
      break;

    /* Handle the SecurityResult message */
    if (!rfbHandleAuthResult(client))
      break;

    result = TRUE;
    break;
  }

  if (cred)
    FreeUserCredential(cred);
  if (mod)
    free(mod);
  if (genmpi)
    gcry_mpi_release(genmpi);
  if (modmpi)
    gcry_mpi_release(modmpi);
  if (respmpi)
    gcry_mpi_release(respmpi);
  if (privmpi)
    gcry_mpi_release(privmpi);
  if (pubmpi)
    gcry_mpi_release(pubmpi);
  if (keympi)
    gcry_mpi_release(keympi);
  if (md5)
    gcry_md_close(md5);
  if (aes)
    gcry_cipher_close(aes);
  return result;
}