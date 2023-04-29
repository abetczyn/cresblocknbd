#ifndef CRESBLOCK_H
#define CRESBLOCK_H
#include <unistd.h>
#include <arpa/inet.h>
#define CRESBLOCK_MAX_DATA 1024
typedef struct
{
    uint32_t size;
    int fd;
} cresblock_dev_t;

int cresblock_initialize(cresblock_dev_t *dev);
int cresblock_write(cresblock_dev_t *dev, const void *buf, uint32_t count, uint32_t offset);
int cresblock_read(cresblock_dev_t *dev, void *buf, uint32_t count, uint32_t offset);
#endif