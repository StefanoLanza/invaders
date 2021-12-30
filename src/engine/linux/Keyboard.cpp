#include "../Keyboard.h"
#include <cstring>

#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <dirent.h>
#include <array>
#include <ncurses.h>

namespace
{

    int prevKeyState[numKeyCodes] = {-1};
    int keyState[numKeyCodes] = {-1};

    const int keyCodeToVKey[numKeyCodes] =
        {
            27, // ESCAPE,
            KEY_ENTER,
            KEY_BACKSPACE,
            KEY_LEFT,
            KEY_RIGHT,
            KEY_UP,
            KEY_DOWN,
            '1',
            '2',
            '3',
            '4',
            '5',
            'A',
            'D'};

    int fd = -1;
    unsigned int getchFrequency = 0; 

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1) / BITS_PER_LONG) + 1)
#define OFF(x) ((x) % BITS_PER_LONG)
#define BIT(x) (1UL << OFF(x))
#define LONG(x) ((x) / BITS_PER_LONG)
#define test_bit(bit, array) ((array[LONG(bit)] >> OFF(bit)) & 1)

int endsWith(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix > lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

void UpdateKeyStatesRaw() {
    struct input_event ev[64];
    int rd = read(fd, ev, sizeof(struct input_event) * 64);
    if (rd >= (int)sizeof(struct input_event))
    {
        for (int i = 0; i < std::size(ev); i++)
        {

            if (ev[i].type == EV_SYN)
            {
                printf("Event: time %ld.%06ld, -------------- %s ------------\n",
                       ev[i].time.tv_sec, ev[i].time.tv_usec, ev[i].code ? "Config Sync" : "Report Sync");
            }
            else if (ev[i].type == EV_MSC && (ev[i].code == MSC_RAW || ev[i].code == MSC_SCAN))
            {
                printf("Event: time %ld.%06ld, type %d, code %d, value %02x\n",
                       ev[i].time.tv_sec, ev[i].time.tv_usec, ev[i].type,

                       ev[i].code,

                       ev[i].value);
            }
            else
            {
                printf("Event: time %ld.%06ld, type %d, code %d, value %d\n",
                       ev[i].time.tv_sec, ev[i].time.tv_usec, ev[i].type,

                       ev[i].code,

                       ev[i].value);
            }
        }
    }
    else
    {

        printf("yyy\n");
        perror("\nevtest: error reading");
        return;
    }
}

void UpdateKeyStatesGetch() {
    for (;;) {
        const int ch = getch();
        if (ch == ERR) {
            break;
        }
        for (size_t i = 0; i < numKeyCodes; ++i)
        {
            if (ch == keyCodeToVKey[i])
            {
                keyState[i] = 1;
                break;
            }
        }
    }
}

}

bool InitKeyboard()
{
    // better be safe than sory: while running other programs, switch user to nobody
    setegid(65534); seteuid(65534);

    const char *inputDirPath = "/dev/input/by-path";
    DIR *inputDir = opendir(inputDirPath);

    if (!inputDir)
    {
        //  DEBUG_LOG("Failed to open input dir\n");
        return false;
    }

    struct dirent *entry = readdir(inputDir);
    unsigned short id[4];
    char name[256] = "Unknown";
    unsigned long bit[EV_MAX][NBITS(KEY_MAX)];
    int version = 0;

    while (entry)
    {
        if (endsWith(entry->d_name, "-event-kbd"))
        {
            char path[512];
            snprintf(path, sizeof path, "%s/%s", inputDirPath, entry->d_name);
            fd = open(path, O_RDONLY | O_NONBLOCK);
            if (fd < 0)
            {
                goto next;
            }
            if (ioctl(fd, EVIOCGVERSION, &version))
            {
                goto errorFileOpened;
            }

            printf("Input driver version is %d.%d.%d\n",
                   version >> 16, (version >> 8) & 0xff, version & 0xff);

            ioctl(fd, EVIOCGID, id);
            printf("Input device ID: bus 0x%x vendor 0x%x product 0x%x version 0x%x\n",
                   id[ID_BUS], id[ID_VENDOR], id[ID_PRODUCT], id[ID_VERSION]);

            ioctl(fd, EVIOCGNAME(sizeof(name)), name);
            printf("Input device name: \"%s\"\n", name);

            memset(bit, 0, sizeof(bit));
            if (ioctl(fd, EVIOCGBIT(0, EV_MAX), bit[0]) < 0)
            {
                goto errorFileOpened;
            }
            printf("Supported events:\n");

            break;
        errorFileOpened:
            close(fd);
            fd = -1;
        }
    next:
        entry = readdir(inputDir);
    }
    closedir(inputDir);
    // now we reclaim those root privileges
  seteuid(0); setegid(0);

  if (fd < 0) {
      printf("Raw keyboard fail. Using getch\n");
      timeout(0); // non blocking input

  }
    return true;
}

void UpdateKeyStates()
{
    std::memcpy(prevKeyState, keyState, sizeof(prevKeyState));
    if (fd >= 0)
    {
        std::memset(keyState, 0, sizeof(prevKeyState));
        UpdateKeyStatesRaw();
    }
    else {
        if ((getchFrequency++ % 10) == 0) {
            std::memset(keyState, 0, sizeof(prevKeyState));
        }
        UpdateKeyStatesGetch();
    }
}

bool KeyPressed(KeyCode keyCode)
{
    return (keyState[(int)keyCode] == 1);
}

bool KeyJustPressed(KeyCode keyCode)
{
    return (keyState[(int)keyCode] == 1) && (prevKeyState[(int)keyCode] == 0);
}

bool KeyJustReleased(KeyCode keyCode)
{
    return (keyState[(int)keyCode] == 0) && (prevKeyState[(int)keyCode] == 1);
}

bool AnyKeyJustPressed()
{
    for (size_t i = 0; i < numKeyCodes; ++i)
    {
        if (KeyJustPressed((KeyCode)i))
        {
            return true;
        }
    }
    return false;
}
