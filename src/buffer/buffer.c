#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "buffer.h"
#include "../sync/sync.h"
#include "../type/type.h"
#include "../index/index.h"

char *buffer_ptr = NULL; //缓存区首地址
char *data_ptr = NULL; //缓存区写入指针
char *need_flush_ptr = NULL; //
char *db_file = NULL; //数据库文件地址
char *offset_char = NULL; //数据块偏移量在磁盘的缓存char数组
char *data_last_ptr = NULL;

OFFSET_TYPE *offset_int = NULL; //数据块偏移量的lld指针，用来赋值
OFFSET_TYPE sync_offset;  //数据偏移量

int value_fd;

int times; //趟数
int not_flush_size; //没有持久化的数据量
int exit_flag; //退出flag
int sleep_time; //子线程休眠时间
int buffer_pool_size;

pthread_t tid;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

const int head_size = sizeof(Head); //数据头大小

static void *pthread_flush();//子线程函数

void buffer_init(char *filename, int buffer_size){
	db_file = filename; //数据库文件地址
	sleep_time = 1;		//刷新线程睡眠时间
	buffer_pool_size = buffer_size;
	
	buffer_ptr = (char *)malloc(buffer_pool_size);//申请缓存池空间
	data_ptr = buffer_ptr; //初始化写入指针
	data_last_ptr = buffer_ptr; //初始化第一趟缓存池末尾指针
	need_flush_ptr = buffer_ptr; //初始化需要覆盖的头指针（它一定从头开始吗？未必吧，这样合理吗？）
	
	value_fd = sync_init(filename);//文件IO初始化
	offset_char = (char *)malloc(OFFSET_TYPE_SIZE); 
	
	
	OFFSET_TYPE *sync_offset_ptr_int; //
	char *sync_offset_ptr_char = (char *)malloc(OFFSET_TYPE_SIZE);

	sync_read(value_fd, sync_offset_ptr_char, OFFSET_TYPE_SIZE, 0);//读取数据offset到char内存
	sync_offset_ptr_int = (OFFSET_TYPE *)sync_offset_ptr_char; // 转换为OFFSET_TYPE变量指针
	sync_offset = *sync_offset_ptr_int;  //赋值给sync_offset变量
	if(sync_offset < OFFSET_TYPE_SIZE || sync_offset < 0){
		sync_offset = OFFSET_TYPE_SIZE;
	}
	
	if(pthread_create(&tid, NULL, pthread_flush, NULL) != 0){
		perror("buffer.c:buffer_init\n");
		return;
	}
}

void buffer_flush(Head *flush_head, char *buffer_pos){
	//开始持久化重合的数据块
	if(flush_head->state == not_flush){ //如果原有的数据块未刷新
		sync_write(value_fd, buffer_pos + head_size, flush_head->info->size, sync_offset); //写入需要刷新的数据块中的数据到磁盘
		flush_head->info->offset = sync_offset; //在数据块信息头中保存数据在磁盘中的位置
		flush_head->info->buf	 = NULL;       //设置在缓存池的地址为空，因为数据已经不在缓存池中了，设置为空方便读取的时候判断
		flush_head->state        = flushed;    //设置数据块的状态为已刷新，但是似乎没啥用，已经被覆盖了
		
		sync_offset += flush_head->info->size; //磁盘偏移量后移插入的数据量
		
		offset_int  = (OFFSET_TYPE *)offset_char; 
		*offset_int = sync_offset;		//写入磁盘数据偏移量到char内存
		sync_write(value_fd, offset_char, OFFSET_TYPE_SIZE, 0); //写入磁盘数据偏移量到数据块文件
		
		//由于已经刷新了这个数据块，就从未刷新数据量中减去它的数据量
		pthread_mutex_lock(&mutex); 
		not_flush_size -= (flush_head->info->size + head_size);
		pthread_mutex_unlock(&mutex);
	}
}
char *min(char *ptr_head, char *ptr_after){
	return ptr_head < ptr_after ? ptr_head : ptr_after;
}

void buffer_put(char *value, VI *value_info){
	int value_size = strlen(value);
	
	// 如果写入的数据库大小大于缓存区大小的话，不管经过多少趟，都是无法写入的，系统应该拒绝写入退出
	if(head_size + value_size > buffer_pool_size){
		printf("buffer.c:buffer_put:data size bigger than buffer pool size\n");
		return;
	}

	Head *insert_head; //保存插入数据块数据头的指针变量

	// 如果写入的数据库超出了buffer区的尾部，说明此趟已经无法存入，需要放在下一趟
	if(data_ptr + head_size + value_size > buffer_ptr + buffer_pool_size){
		// 设置是否为第一趟为假
		pthread_mutex_lock(&mutex);
		times ++;
		pthread_mutex_unlock(&mutex);
		//重置写入指针到缓存区首部，重新开始写入
		data_last_ptr = data_ptr;
		data_ptr = buffer_ptr;
		need_flush_ptr = buffer_ptr;
	}
	
	if(times > 0){
		Head *flush_head = NULL;
		/*
			如果原有的数据块与要插入的数据块重合了：原有数据块的头指针在插入数据块尾指针前面
			且原有数据块必须有数据：原有数据块的头指针在缓冲区末尾指针前面
			故推出，原有数据的头指针，必须比插入数据的尾指针和缓存区尾指针都小，也就是小于它们的最小值
		*/
		while(need_flush_ptr < min((data_ptr + head_size + value_size), data_last_ptr)){
			flush_head = (Head *)need_flush_ptr; //从内存中提取原有的数据块的数据头结构体数据
			//开始持久化重合的数据块
			buffer_flush(flush_head, need_flush_ptr);
			
			//持久化完成，移动到下一个原有数据块
			need_flush_ptr += head_size + flush_head->info->size;
		}
		//与插入数据重合的原有数据已经全部持久化完毕，开始真正插入数据
	}
	
	//从要写入的空间中划分一部分空间给head结构体
	insert_head = (Head *)data_ptr;
	//赋值数据块大小给数据信息结构体
	value_info->size = value_size;
	//赋值缓存区首地址给数据信息结构体
	value_info->buf = data_ptr;
	//赋值数据信息结构体给数据头结构体
	insert_head->info = value_info;
	//把数据块状态设置为未刷新
	insert_head->state = not_flush;

	//移动写入指针，准备写入数据
	data_ptr += head_size;

	//写入数据并且移动指针，把指针指向数据尾部，方便下一次写入
	memcpy(data_ptr, value, value_size);
	data_ptr += value_size;

	pthread_mutex_lock(&mutex);
	not_flush_size += (head_size + value_size);
	pthread_mutex_unlock(&mutex);
	//TODO:未刷新计数加一
}

void buffer_get(char *value, VI* value_info){
	Head *head;
	if(value_info->size == 0){
		printf("buffer.c:buffer_get:value info has no value size\n");
		return;
	}

	if(value_info->buf != NULL){
		head = (Head *)value_info->buf;
		if(head->state != unavail){
			memcpy(value, value_info->buf + head_size, value_info->size);
		}else{
			printf("buffer.c:buffer_get:value is unavail, maybe be deleted\n");
		}
	}else{
		if(value_info->offset >= OFFSET_TYPE_SIZE){
			sync_read(value_fd, value, value_info->size, value_info->offset);
		}
		else{
			printf("buffer.c:buffer_get:value not in buffer and disk\n");
		}
	}
}

void buffer_delete(VI* value_info){
	Head *head;
	if(value_info->buf == NULL){
		printf("buffer.c:buffer_delete:data is not in the buffer pool\n");
		return;
	}

	head = (Head*)value_info->buf;
	head->state = unavail;
}

void buffer_exit(){
	exit_flag = 1;
	if(pthread_join(tid, NULL) != 0){
		perror("buffer.c:buffer_exit\n");
		return;
	}
	sync_exit(value_fd);
}

void buffer_flushAll(){
	Head *need_flush_head;
	char *start_ptr;
	start_ptr = buffer_ptr;
	
	while(need_flush_ptr < data_last_ptr){
		need_flush_head = (Head *)need_flush_ptr;
		if(need_flush_head->state == not_flush){
			buffer_flush(need_flush_head, need_flush_ptr);
			need_flush_head->state = flushed;
		}
		need_flush_ptr += head_size + need_flush_head->info->size; 
	}
	
	while(start_ptr < data_ptr){
		need_flush_head = (Head *)start_ptr;
		if(need_flush_head->state == not_flush){
			buffer_flush(need_flush_head, start_ptr);
			need_flush_head->state = flushed;
		}
		start_ptr += head_size + need_flush_head->info->size;
	}
}


static void* pthread_flush(){	
	while(!exit_flag || not_flush_size > 0){
		if(not_flush_size >= FLUSH_HORIZON || exit_flag){
			buffer_flushAll();
			pthread_mutex_lock(&mutex);
			times = 0;
			not_flush_size = 0;
			pthread_mutex_unlock(&mutex);
		}
		else
		{
			sleep(sleep_time);
		}
	}
	free(buffer_ptr);
	pthread_exit(NULL);
}