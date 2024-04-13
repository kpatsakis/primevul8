static void evdev_detach_client(struct evdev *evdev,
				struct evdev_client *client)
{
	spin_lock(&evdev->client_lock);
	list_del_rcu(&client->node);
	spin_unlock(&evdev->client_lock);
	synchronize_rcu();
}