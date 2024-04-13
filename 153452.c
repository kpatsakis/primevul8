save_entry(ScanEnv* env, enum SaveType type, int* id)
{
  int nid = env->save_num;

  env->save_num++;
  *id = nid;
  return 0;
}