#include <M5Unified.h>
#include <stdint.h>

extern "C"
{
#include "src/doomgeneric/doomkeys.h"
#include "src/doomgeneric/doomgeneric.h"
}

// ---- 設定 ----
#define USE_IMU 1         // 傾きで移動。0にするとボタンのみ
#define TILT_THRESH 0.35f // 傾き判定のしきい値(g)
#define TILT_SWAP_XY 1    // 前後と左右が逆なら 1
#define TILT_INV_X 0      // 左右が逆なら 1
#define TILT_INV_Y 1      // 前後が逆なら 1

// ---- 画面 ----
static int dstW, dstH;
static uint16_t xmap[240];
static uint16_t linebuf[240];

static inline uint16_t rgb565(uint32_t p)
{
    uint8_t r = (p >> 16) & 0xFF;
    uint8_t g = (p >> 8) & 0xFF;
    uint8_t b = p & 0xFF;
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// ---- キー入力キュー ----
struct KeyEv
{
    uint8_t pressed;
    uint8_t key;
};
static KeyEv keyq[32];
static uint8_t qhead = 0, qtail = 0;

static void pushKey(bool pressed, uint8_t key)
{
    uint8_t next = (qhead + 1) % 32;
    if (next == qtail)
        return; // 満杯なら捨てる
    keyq[qhead] = {(uint8_t)pressed, key};
    qhead = next;
}

// 状態が変わったときだけイベントを積む
static void setKey(int idx, bool down, uint8_t key)
{
    static bool state[16] = {false};
    if (state[idx] != down)
    {
        state[idx] = down;
        pushKey(down, key);
    }
}

static void pollInput()
{
    M5.update();

    setKey(0, M5.BtnA.isPressed(), KEY_FIRE);
    setKey(1, M5.BtnB.isPressed(), KEY_USE);
    setKey(2, M5.BtnB.isPressed(), KEY_ENTER);    // メニュー決定用
    setKey(3, M5.BtnPWR.isPressed(), KEY_ESCAPE); // メニュー開閉

#if USE_IMU
    float ax = 0, ay = 0, az = 0;
    if (M5.Imu.getAccel(&ax, &ay, &az))
    {
#if TILT_SWAP_XY
        float t = ax;
        ax = ay;
        ay = t;
#endif
#if TILT_INV_X
        ax = -ax;
#endif
#if TILT_INV_Y
        ay = -ay;
#endif
        setKey(4, ax < -TILT_THRESH, KEY_LEFTARROW);
        setKey(5, ax > TILT_THRESH, KEY_RIGHTARROW);
        setKey(6, ay > TILT_THRESH, KEY_UPARROW);
        setKey(7, ay < -TILT_THRESH, KEY_DOWNARROW);
    }
#endif
}

extern "C"
{

    void DG_Init()
    {
        auto cfg = M5.config();
        M5.begin(cfg);

        M5.Display.setRotation(1); // 横向き 240x135
        M5.Display.fillScreen(TFT_BLACK);
        dstW = M5.Display.width();
        dstH = M5.Display.height();
        if (dstW > 240)
            dstW = 240;

        for (int x = 0; x < dstW; x++)
            xmap[x] = x * DOOMGENERIC_RESX / dstW;
    }

    void DG_DrawFrame()
    {
        const uint32_t *src = (const uint32_t *)DG_ScreenBuffer;

        M5.Display.startWrite();
        for (int y = 0; y < dstH; y++)
        {
            const uint32_t *row = src + (y * DOOMGENERIC_RESY / dstH) * DOOMGENERIC_RESX;
            for (int x = 0; x < dstW; x++)
                linebuf[x] = rgb565(row[xmap[x]]);
            M5.Display.pushImage(0, y, dstW, 1, linebuf);
        }
        M5.Display.endWrite();

        pollInput(); // 1フレームに1回入力を更新
    }

    void DG_SleepMs(uint32_t ms)
    {
        delay(ms);
    }

    uint32_t DG_GetTicksMs()
    {
        return millis();
    }

    int DG_GetKey(int *pressed, unsigned char *key)
    {
        if (qtail == qhead)
            return 0;
        *pressed = keyq[qtail].pressed;
        *key = keyq[qtail].key;
        qtail = (qtail + 1) % 32;
        return 1;
    }

    void DG_SetWindowTitle(const char *title)
    {
    }
}