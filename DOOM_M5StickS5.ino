// 依存関係は付属ファイルにて。
//

#include <M5Unified.h>

extern "C"
{
#include "src/doomgeneric/doomgeneric.h"
}

// SET_LOOP_TASK_STACK_SIZE(24 * 1024); // 重要: DOOM は深いスタックを使う
static char *doom_argv[] = {(char *)"doom", (char *)"-iwad", (char *)"doom1.wad", NULL};

void setup()
{
    Serial.begin(115200);
    delay(1500); // USB CDC の接続待ち
    Serial.println("boot");

    auto cfg = M5.config();
    M5.begin(cfg);
    M5.Display.setRotation(1);
    M5.Display.setSwapBytes(true);
    M5.Display.setBrightness(128);
    M5.Display.fillScreen(TFT_RED); // 赤が出れば画面は生きている
    Serial.println("display ok");
    delay(500);

    Serial.printf("DRAM free: %u, PSRAM free: %u\n",
                  heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
                  heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

    Serial.println("doomgeneric_Create...");
    doomgeneric_Create(3, doom_argv);
    Serial.println("create done");
}

void loop()
{
    doomgeneric_Tick();
}