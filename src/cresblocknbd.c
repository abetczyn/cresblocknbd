#define NBDKIT_API_VERSION 2
#include <nbdkit-plugin.h>
#define THREAD_MODEL NBDKIT_THREAD_MODEL_SERIALIZE_ALL_REQUESTS
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#include <string.h>
#include <stdlib.h>
#include "cresblock.h"
#include "tcpclient.h"

typedef struct {
    uint32_t offset;
    uint32_t count;
    cresblock_dev_t *dev;
} command_t;

cresblock_dev_t devices[10];
size_t deviceCount = 0;
u_int16_t port = 60010;
int64_t size = 0;

static int cresConfig(const char *key, const char *value)
{
    if(strcmp("servers", key) == 0)
    {
        devices[deviceCount].fd = client_connect(value, port);
        deviceCount += cresblock_initialize(devices + deviceCount) == 0;
    }
    else if(strcmp("port", key) == 0)
    {
        port = atoi(value);
        if(port == 0)
        {
            nbdkit_error("Invalid port value");
            return -1;
        }
    }
    return 0;
}

static int cresConfigComplete()
{
    int i;
    for(i = 0;i < deviceCount;i++)
    {
        nbdkit_debug("Saw device %d\tsize = %lu\n", i, devices[i].size);
        size += devices[i].size;
    }
    if(size <= 0)
    {
        nbdkit_error("Size cannot be 0");
        return -1;
    }
}

static void * cresOpen(int readonly)
{
    return NBDKIT_HANDLE_NOT_NEEDED;
}

static void cresUnload()
{
    int i;
    for(i = 0;i < deviceCount;i++)
    {
        client_disconnect(devices[i].fd);
    }
}

static int64_t cresSize(void *handle)
{
    return size;
}

static uint32_t getCommands(uint32_t count, uint64_t offset, command_t *commands)
{
    uint32_t commandCount = 0;
    //find first device with offset
    cresblock_dev_t *currentDevice;
    for(currentDevice = devices; currentDevice->size <= offset; currentDevice++)
    {
        offset -= currentDevice->size;
    }

    while(count > 0)
    {
        //dataToSend is min(count, currentDevice->size - offset, CRESBLOCK_MAX_DATA)
        uint32_t dataToSend = MIN(MIN(count, currentDevice->size - offset), CRESBLOCK_MAX_DATA);
        commands[commandCount].dev = currentDevice;
        commands[commandCount].count = dataToSend;
        commands[commandCount].offset = offset;
        count -= dataToSend;
        offset = 0;
        currentDevice++;
        commandCount++;
    }

    return commandCount;
}

static int cresRead(void *handle, void *buf, uint32_t count, uint64_t offset, uint32_t flags)
{
    command_t commands[deviceCount];
    u_int32_t commandCount = getCommands(count, offset, commands);
    u_int32_t i;
    for(i = 0;i < commandCount;i++)
    {
        if(cresblock_read(commands[i].dev, buf, commands[i].count, commands[i].offset))
            return -1;
    }
    return 0;
}

static int cresWrite(void *handle, const void *buf, uint32_t count, uint64_t offset, uint32_t flags)
{
    command_t commands[deviceCount];
    u_int32_t commandCount = getCommands(count, offset, commands);
    u_int32_t i;
    for(i = 0;i < commandCount;i++)
    {
        if(cresblock_write(commands[i].dev, buf, commands[i].count, commands[i].offset))
            return -1;
    }
    return 0;
}

 static struct nbdkit_plugin plugin = {
   .name              = "CresPlugin",
   .magic_config_key  = "device",
   .open              = cresOpen,
   .get_size          = cresSize,
   .config            = cresConfig,
   .config_complete   = cresConfigComplete,
   .unload            = cresUnload,
   .pread             = cresRead,
   .pwrite            = cresWrite
 };
 NBDKIT_REGISTER_PLUGIN(plugin)