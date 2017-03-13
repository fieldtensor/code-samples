#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <XKit/List.hpp>
#include <XKit/Util.hpp>
#include <XKit/AutoreleasePool.hpp>

class WriteRecord : public XK::Object
{
private:
    double _startTime;
    double _stopTime;
    size_t _bytesOut;

public:
    WriteRecord(double start, double stop, size_t bytes);

    double startTime() { return _startTime; }
    double stopTime() { return _stopTime; }
    size_t bytesOut() { return _bytesOut; }
};

WriteRecord::WriteRecord(
    double start, double stop, size_t bytes)
{
    _startTime = start;
    _stopTime = stop;
    _bytesOut = bytes;
}

void
PruneWriteLog(XK::MList* writeLog, double secs)
{
    double now = XK::Clock() / (double)XK::ClockRes();

    const XK::ListNode* node = writeLog->head();
    while(node != NULL)
    {
        const XK::ListNode* next = node->next();

        WriteRecord* rec = (WriteRecord*)node->value();
        double dt = now - rec->startTime();
        if( dt > secs )
            writeLog->popFront();
        else
            break;

        node = next;
    }
}

void
ReportWriteLog(
    XK::MList* writeLog, size_t totalOutput, size_t size)
{
    size_t byteCount = 0;
    double totalTime = 0.0;

    for(const XK::ListNode* node = writeLog->head();
        node != NULL;
        node = node->next())
    {
        WriteRecord* rec = (WriteRecord*)node->value();
        byteCount += rec->bytesOut();
        totalTime += rec->stopTime() - rec->startTime();
    }

    double bps = byteCount / totalTime;
    double mbps = bps / 1024.0;

    printf("%.2f%% %.0f MB/s\n", 100.0 * (totalOutput/(double)size), mbps);
}

int
LargeWriteRun(
    const char* fileName,
    size_t size,
    size_t hunkSize,
    bool alwaySync)
{
    int fd = open(fileName, O_CREAT | O_TRUNC | O_WRONLY, 0644);

    if( fd == -1 )
    {
        printf("ERRR: open() failed: %s\n", strerror(errno));
        return 1;
    }

    char* buff = (char*)malloc(hunkSize);
    for(size_t i = 0; i < hunkSize; i++)
        buff[i] = rand() % 255;

    XK::MList* writeLog = XK::MList::make();

    double clockRes = XK::ClockRes();

    double lastReportTime = XK::Clock() / clockRes;

    size_t totalOutput = 0;
    while(totalOutput < size)
    {
        double before = XK::Clock() / clockRes;

        ssize_t ret;
        while(true)
        {
            ret = write(fd, buff, hunkSize);
            if( ret == -1 && errno == EINTR )
                continue;
            break;
        }

        if( ret == -1 )
        {
            printf("write() failure: %s", strerror(errno));
            return 1;
        }

        if( alwaySync )
        {
            if( fsync(fd) == -1 )
            {
                printf("fsync() failure: %s", strerror(errno));
                return 1;
            }
        }

        double after = XK::Clock() / clockRes;

        totalOutput += ret;

        WriteRecord* rec = new WriteRecord(before, after, ret);
        writeLog->pushBack(rec);

        PruneWriteLog(writeLog, 2.0);

        double now = XK::Clock() / clockRes;
        if( now - lastReportTime > 2.0 )
        {
            lastReportTime = now;
            ReportWriteLog(writeLog, totalOutput, size);
        }
    }

    PruneWriteLog(writeLog, 2.0);
    ReportWriteLog(writeLog, totalOutput, size);

    double before = XK::Clock() / clockRes;
    if( fsync(fd) == -1 )
    {
        printf("fsync() failure: %s", strerror(errno));
        return 1;
    }
    double after = XK::Clock() / clockRes;

    printf("fsync: %f\n", after - before);
}

int
LargeWrite(int argc, const char** argv)
{
    const char* fileName = NULL;
    size_t hunkSize = 4 * 1024;
    size_t size = 1024 * 1024 * 100;
    bool alwaySync = false;

    size_t idx = 2;
    for(idx = 2; idx < argc; idx++)
    {
        if( ! strcmp(argv[idx], "--file-name") )
        {
            if( idx >= argc )
            {
                printf("ERROR: --file-name requires a pat\n");
                return 1;
            }

            fileName = argv[++idx];
        }
        else if( ! strcmp(argv[idx], "--hunk-size") )
        {
            const char* arg = argv[++idx];
            hunkSize = atoi(arg);
        }
        else if( ! strcmp(argv[idx], "--size-gb") )
        {
            const char* arg = argv[++idx];
            size_t factor = 1024 * 1024 * 1024;
            size = atoi(arg);
            size *= factor;
        }
        else if( ! strcmp(argv[idx], "--size-mb") )
        {
            const char* arg = argv[++idx];
            size_t factor = 1024 * 1024;
            size = atoi(arg);
            size *= factor;
        }
        else if( ! strcmp(argv[idx], "--size-kb") )
        {
            const char* arg = argv[++idx];
            size_t factor = 1024;
            size = atoi(arg);
            size *= factor;
        }
        else if( ! strcmp(argv[idx], "--sync") )
        {
            alwaySync = true;
        }
    }

    if( fileName == NULL )
    {
        printf("ERROR: --large-write requires a file name\n");
        exit(1);
    }

    printf("File: %s\n", fileName);
    printf("Size: %zu\n", size);
    printf("Hunk: %zu\n", hunkSize);
    printf("Sync: %s\n", alwaySync ? "yes" : "no");

    return LargeWriteRun(fileName, size, hunkSize, alwaySync);
}

int
InfiniteRead(int argc, const char** argv)
{
    printf("InfiniteRead\n");
    return 0;
}

int
main(int argc, const char** argv)
{

    XK::AutoreleasePoolPush();
    if( argc < 2 )
    {
        printf("Usage: %s <args>\n", argv[0]);
        exit(1);
    }

    if( ! strcmp(argv[1], "--large-write") )
    {
        return LargeWrite(argc, argv);
    }
    else if( ! strcmp(argv[1], "--infinite-read") )
    {
        return InfiniteRead(argc, argv);
    }

    return 0;
}
