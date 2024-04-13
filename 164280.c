card_id_show_attr(struct device *dev,
		  struct device_attribute *attr, char *buf)
{
	struct snd_card *card = container_of(dev, struct snd_card, card_dev);
	return scnprintf(buf, PAGE_SIZE, "%s\n", card->id);
}