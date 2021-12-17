#include "../Keyboard.h"
#include <cstring>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <array>

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
}

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1) / BITS_PER_LONG) + 1)
#define OFF(x) ((x) % BITS_PER_LONG)
#define BIT(x) (1UL << OFF(x))
#define LONG(x) ((x) / BITS_PER_LONG)
#define test_bit(bit, array) ((array[LONG(bit)] >> OFF(bit)) & 1)

bool InitKeyboard()
{
    fd = open("/dev/input/event0", O_RDONLY);
    if (fd < 0)
    {
        return false;
    }
    int version = 0;
    if (ioctl(fd, EVIOCGVERSION, &version))
    {
        // perror("evtest: can't get version");
        return false;
    }

    printf("Input driver version is %d.%d.%d\n",
           version >> 16, (version >> 8) & 0xff, version & 0xff);

    unsigned short id[4];
    ioctl(fd, EVIOCGID, id);
    printf("Input device ID: bus 0x%x vendor 0x%x product 0x%x version 0x%x\n",
           id[ID_BUS], id[ID_VENDOR], id[ID_PRODUCT], id[ID_VERSION]);

    char name[256] = "Unknown";
    ioctl(fd, EVIOCGNAME(sizeof(name)), name);
    printf("Input device name: \"%s\"\n", name);

    unsigned long bit[EV_MAX][NBITS(KEY_MAX)];
    memset(bit, 0, sizeof(bit));
    ioctl(fd, EVIOCGBIT(0, EV_MAX), bit[0]);
    printf("Supported events:\n");

    return true;
}

void UpdateKeyStates()
{
    if (fd < 0)
    {
        return;
    }
    std::memcpy(prevKeyState, keyState, sizeof(prevKeyState));
    std::memset(keyState, 0, sizeof(prevKeyState));

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
    #if 0
    const int ch = getch();
    for (size_t i = 0; i < numKeyCodes; ++i)
    {
        if (ch == keyCodeToVKey[i])
        {
            keyState[i] = 1;
            break;
        }
    }
    #endif
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
    bool res = false;
    for (size_t i = 0; i < numKeyCodes; ++i)
    {
        if (KeyJustPressed((KeyCode)i))
        {
            res = true;
            break;
        }
    }
    return res;
}
