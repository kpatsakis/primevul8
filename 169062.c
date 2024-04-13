static void fn_spawn_con(struct vc_data *vc)
{
	spin_lock(&vt_spawn_con.lock);
	if (vt_spawn_con.pid)
		if (kill_pid(vt_spawn_con.pid, vt_spawn_con.sig, 1)) {
			put_pid(vt_spawn_con.pid);
			vt_spawn_con.pid = NULL;
		}
	spin_unlock(&vt_spawn_con.lock);
}