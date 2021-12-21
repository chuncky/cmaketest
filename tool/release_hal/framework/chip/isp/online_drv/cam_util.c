#include "cam_util.h"
#include "cam_list.h"
#include "../camera_common_log.h"

void uudelay(int us)
{
	volatile uint32_t i;
	for(i=0; i<us*100;i++)
		i = i+1;
}

void mdelay(int ms){
	volatile uint32_t i;
	for(i=0; i<ms;i++)
		uudelay(10);
}


void msleep(unsigned int i)
{
//	OSATaskSleep(i);
	mdelay(i);
	return;
}

void set_bit(int nr, unsigned long *addr)
{
	addr[nr / BITS_PER_LONG] |= 1UL << (nr % BITS_PER_LONG);
}

void clear_bit(int nr, unsigned long *addr)
{
	addr[nr / BITS_PER_LONG] &= ~(1UL << (nr % BITS_PER_LONG));
}


static LOG_GROUP log_group;
static int log_cnt = 0;

int cam_mem_log_init(void)
{
	int ret = 0;

	if (GROUP_INITED == log_group.group_init) {
		return ret;
	}

	log_group.log_queue = UOS_MALLOC(sizeof(LOG_QUEUE));
	if (!log_group.log_queue) {
		CAMLOGE("irq_printk_init: UOS_MALLOC failed!");
		return -1;
	}

	INIT_LIST_HEAD(&log_group.log_queue->log_head);
	log_group.queue_lock = UOS_NewMutex("cam_log");
	log_group.group_init = GROUP_INITED;
	log_group.log_queue_cnt = 0;
	log_cnt = 0;
	return ret;
}

void cam_mem_log_print(void)
{
	int ret = 0;
	char *out;
	int size = 0;
	LOG_QUEUE *log_queue = NULL;
	struct list_head *list_i;

	UOS_TakeMutex(log_group.queue_lock);

	if (log_group.log_queue_cnt > 0) {

		list_for_each(list_i, &log_group.log_queue->log_head){
			log_queue = list_entry(list_i,LOG_QUEUE,log_head);
			if (NULL == log_queue) {
				goto end_1;
			}
			CAMLOGV("%d irq_printk:%s", log_queue->timestamp, log_queue->format);
			list_del_init(&log_queue->log_head);
			log_group.log_queue_cnt--;
			ret = UOS_FREE((PVOID)log_queue);
			if (!ret) {
				CAMLOGE("irq_printk:free memory fail!");
				goto end_1;
			}
		}
	}

end_1:
	UOS_ReleaseMutex(log_group.queue_lock);
	return;
}

int cam_mem_log_send(const char *format)
{
	int ret = 0;
	LOG_QUEUE *log_queue = NULL;
	size_t len;

	if (NULL == format) {
		return -1;
	}

	if (MAX_LOG_SZIE < strlen(format)) {
		return -2;
	}

	UOS_TakeMutex(log_group.queue_lock);

	if (log_cnt < MAX_QUEUE_SZIE) {
		if (0 == log_group.log_queue_cnt) {
			log_group.log_queue->timestamp = OSAGetTicks();
			memcpy(log_group.log_queue->format, format, strlen(format));
			log_group.log_queue_cnt++;
		} else {
			log_queue = (LOG_QUEUE*)UOS_MALLOC(sizeof(LOG_QUEUE));
			if (!log_queue) {
				goto end_1;
			}

			log_queue->timestamp = OSAGetTicks();
			memcpy(log_queue->format, format, strlen(format));
			list_add_tail(&log_queue->log_head, &log_group.log_queue->log_head);
			log_group.log_queue_cnt++;
		}
	} else {
		unsigned int index = log_cnt % MAX_QUEUE_SZIE;
		struct list_head *list_i;
		list_for_each(list_i, &log_group.log_queue->log_head){
			log_queue = list_entry(list_i,LOG_QUEUE,log_head);
			if (index <= 0)
				break;
			index--;
		}

		if (NULL == log_queue) {
			goto end_1;
		}

		//change element value
		log_queue->timestamp = OSAGetTicks();
		memcpy(log_queue->format, format, strlen(format));
	}

	log_cnt++;

end_1:
	UOS_ReleaseMutex(log_group.queue_lock);

	return ret;
}

