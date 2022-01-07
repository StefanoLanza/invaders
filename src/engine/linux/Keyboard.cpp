#include "../Keyboard.h"
#include <cstring>

#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <dirent.h>
#include <array>

namespace
{

    int prevKeyState[numKeyCodes] = {-1};
    int keyState[numKeyCodes] = {-1};

    const int keyCodeToVKey[numKeyCodes] =
        {
            KEY_ESC,
            KEY_ENTER,
            KEY_BACKSPACE,
            KEY_LEFT,
            KEY_RIGHT,
            KEY_UP,
            KEY_DOWN,
            KEY_LEFTCTRL,
            KEY_RIGHTCTRL,
            KEY_1,
            KEY_2,
            KEY_3,
            KEY_4,
            KEY_5,
            KEY_A,
            KEY_D
        };

    int fd = -1;

int EndsWith(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix > lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

#define KEY_RELEASE 0
#define KEY_PRESS 1
#define KEY_KEEPING_PRESSED 2

void UpdateKeyStatesRaw() {
    struct input_event ev[64];
    const int rd = read(fd, ev, sizeof ev);
    if (rd >= (int)sizeof(struct input_event))
    {
        for (size_t i = 0; i < std::size(ev); i++)
        {
            if (ev[i].type == EV_KEY)
            {
                for (size_t j = 0; j < numKeyCodes; ++j)
                {
                    if (ev[i].code == keyCodeToVKey[j])
                    {
                       keyState[j] = ev[i].value == KEY_RELEASE ? 0 : 1;
                       break;
                    }
                }
            }
        }
    }
}

}

bool InitKeyboard()
{
    printf("Initializing keyboard\n");

    const char *inputDirPath = "/dev/input/by-path";
    DIR *inputDir = opendir(inputDirPath);
    if (!inputDir)
    {
        printf("Failed to open input dir %s\n", inputDirPath);
        return false;
    }

    struct dirent *entry = readdir(inputDir);
    int version = 0;

    while (entry)
    {
        if (EndsWith(entry->d_name, "-event-kbd"))
        {
            unsigned short id[4];
            char name[256] = "Unknown";

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

            break;
        errorFileOpened:
            close(fd);
            fd = -1;
        }
    next:
        entry = readdir(inputDir);
    }
    closedir(inputDir);

    if (fd < 0) 
    {
        printf("Raw keyboard fail. Run as administrator to prevent this error.\n");
        return false;
    }
    return true;
}

void UpdateKeyStates()
{
    std::memcpy(prevKeyState, keyState, sizeof(prevKeyState));
    if (fd >= 0)
    {
        UpdateKeyStatesRaw();
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
        if (KeyJustPressed(static_cast<KeyCode>(i)))
        {
            return true;
        }
    }
    return false;
}
