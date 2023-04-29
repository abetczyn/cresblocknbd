#include "cresblock.h"
#include <string.h>
#include <tcpclient.h>
#include <arpa/inet.h>

/*
    | HEADER_MAGIC (4 bytes) | cmd (4 bytes) | count (4 bytes) | offset (4 bytes) | buffer (`count` bytes) |
*/
typedef struct __attribute__((packed))
{
    uint32_t magic;
    uint32_t cmd;
    uint32_t count;
    uint32_t offset;
} cresPacketHeader_t;

#define HEADER_MAGIC (htonl(0xC4E5B10C))

int cresblock_write(cresblock_dev_t *dev, const void *buf, uint32_t count, uint32_t offset)
{
    /*
    | HEADER_MAGIC (4 bytes) | cmd (4 bytes) | count (4 bytes) | offset (4 bytes) | buffer (`count` bytes) |
    */

    // build the header packet
    cresPacketHeader_t packetHeaderTx, packetHeaderRx;
    packetHeaderTx.magic = HEADER_MAGIC;
    packetHeaderTx.cmd = htonl((uint32_t)'W');
    packetHeaderTx.count = htonl(count);
    packetHeaderTx.offset = htonl(offset);

    //send header
    if(client_send(dev->fd, &packetHeaderTx, sizeof(packetHeaderTx)))
        return -1;

    //send data
    if(client_send(dev->fd, buf, count))
        return -1;
    
    //receive header
    if(client_receive(dev->fd, &packetHeaderRx, sizeof(packetHeaderRx)))
        return -1;
    
    //check received header
    uint32_t check = packetHeaderTx.magic & ~packetHeaderRx.magic;
    check |= packetHeaderTx.cmd & ~packetHeaderRx.cmd;
    check |= packetHeaderTx.count & ~packetHeaderRx.count;
    check |= packetHeaderTx.offset & ~packetHeaderRx.offset;
    if(check)
        return -1;

    //success
    return 0;
}

int cresblock_read(cresblock_dev_t *dev, void *buf, uint32_t count, uint32_t offset)
{
    /*
    | HEADER_MAGIC (4 bytes) | cmd (4 bytes) | count (4 bytes) | offset (4 bytes) | buffer (`count` bytes) |
    */

    // build the header packet
    cresPacketHeader_t packetHeaderTx, packetHeaderRx;
    packetHeaderTx.magic = HEADER_MAGIC;
    packetHeaderTx.cmd = htonl((uint32_t)'R');
    packetHeaderTx.count = htonl(count);
    packetHeaderTx.offset = htonl(offset);

    //send header
    if(client_send(dev->fd, &packetHeaderTx, sizeof(packetHeaderTx)))
        return -1;
    
    //receive header
    if(client_receive(dev->fd, &packetHeaderRx, sizeof(packetHeaderRx)))
        return -1;
    
    //check received header
    uint32_t check = packetHeaderTx.magic & ~packetHeaderRx.magic;
    check |= packetHeaderTx.cmd & ~packetHeaderRx.cmd;
    check |= packetHeaderTx.count & ~packetHeaderRx.cmd;
    check |= packetHeaderTx.offset & ~packetHeaderRx.offset;
    if(check)
        return -1;
    
    //receive data
    if(client_receive(dev->fd, buf, count))
        return -1;
    
    //success
    return 0;
}

int cresblock_initialize(cresblock_dev_t *dev)
{
    int ret = client_receive(dev->fd, &dev->size, sizeof(dev->size));
    dev->size = ntohl(dev->size);
    return ret;
}
