#include "hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include "battery.h"

#define RAW_PATH            "/sys/bus/iio/devices/iio:device0/in_voltage4_raw"
#define SCALE_PATH          "/sys/bus/iio/devices/iio:device0/in_voltage_scale"
#define OFFSET_PATH         "/sys/bus/iio/devices/iio:device0/in_voltage_offset"


/* PG4: low = charging, high = full; blink = no battery */
uint8_t chrg_level_read(void) {
    return read_gpio_value(PG4_NAME);
}

static int read_int(const char *path, int *out) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    if (fscanf(f, "%d", out) != 1) { fclose(f); return -1; }
    fclose(f);
    return 0;
}

uint16_t battery_adc_read(void)
{
    int raw = 0;
    if (read_int(RAW_PATH, &raw) < 0)
    {
        return 0;
    }
    return (uint16_t)(raw);
}


/*
    index:0-100  value:mv
    Mapping rule:
    - 5%-90%: baseline uniform runtime per displayed 5% step.
    - 90%-95% and 95%-100%: uniform within this top range, each 5% step lasts
      about 15% longer than a baseline 5% step.
    - From the 18-segment run: baseline 5% ~= 316.3s, top 5% ~= 363.7s.
*/
const uint16_t battery_voltage_table[100+1] = {
    3370,       //0%
    3398,       //1%, +28
    3425,       //2%, +27
    3452,       //3%, +27
    3479,       //4%, +27
    3507,       //5%, +28
    3535,       //6%, +28
    3562,       //7%, +27
    3589,       //8%, +27
    3616,       //9%, +27
    3644,       //10%, +28
    3653,       //11%, +9
    3659,       //12%, +6
    3665,       //13%, +6
    3671,       //14%, +6
    3677,       //15%, +6
    3683,       //16%, +6
    3689,       //17%, +6
    3695,       //18%, +6
    3701,       //19%, +6
    3707,       //20%, +6
    3712,       //21%, +5
    3716,       //22%, +4
    3720,       //23%, +4
    3724,       //24%, +4
    3728,       //25%, +4
    3731,       //26%, +3
    3735,       //27%, +4
    3739,       //28%, +4
    3743,       //29%, +4
    3747,       //30%, +4
    3751,       //31%, +4
    3753,       //32%, +2
    3756,       //33%, +3
    3758,       //34%, +2
    3761,       //35%, +3
    3763,       //36%, +2
    3766,       //37%, +3
    3769,       //38%, +3
    3772,       //39%, +3
    3774,       //40%, +2
    3777,       //41%, +3
    3780,       //42%, +3
    3782,       //43%, +2
    3785,       //44%, +3
    3787,       //45%, +2
    3790,       //46%, +3
    3793,       //47%, +3
    3795,       //48%, +2
    3798,       //49%, +3
    3800,       //50%, +2
    3803,       //51%, +3
    3807,       //52%, +4
    3811,       //53%, +4
    3815,       //54%, +4
    3819,       //55%, +4
    3823,       //56%, +4
    3826,       //57%, +3
    3830,       //58%, +4
    3834,       //59%, +4
    3837,       //60%, +3
    3842,       //61%, +5
    3847,       //62%, +5
    3852,       //63%, +5
    3858,       //64%, +6
    3863,       //65%, +5
    3869,       //66%, +6
    3875,       //67%, +6
    3880,       //68%, +5
    3886,       //69%, +6
    3891,       //70%, +5
    3897,       //71%, +6
    3903,       //72%, +6
    3909,       //73%, +6
    3914,       //74%, +5
    3920,       //75%, +6
    3927,       //76%, +7
    3933,       //77%, +6
    3939,       //78%, +6
    3945,       //79%, +6
    3951,       //80%, +6
    3957,       //81%, +6
    3963,       //82%, +6
    3969,       //83%, +6
    3975,       //84%, +6
    3981,       //85%, +6
    3988,       //86%, +7
    3995,       //87%, +7
    4002,       //88%, +7
    4009,       //89%, +7
    4016,       //90%, +7
    4024,       //91%, +8
    4032,       //92%, +8
    4043,       //93%, +11
    4054,       //94%, +11
    4067,       //95%, +13
    4081,       //96%, +14
    4096,       //97%, +15
    4113,       //98%, +17
    4131,       //99%, +18
    4150,       //100%, +19
};

/* Calib note: 5%-90% is the baseline; 90%-100% is weighted 15% longer. */
