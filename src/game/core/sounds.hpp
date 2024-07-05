#pragma once
#include "util/mem-types.hpp"
#include "game/util/store.hpp"
#include "sound/sound-mgr.hpp"

struct Sound;
struct Device_name;

namespace hpw {
inline Unique<Sound_mgr> sound_mgr {}; // звуковая система
inline Unique<Store<Sound>> store_sound {}; // связывает имена треков и пути к трекам из архива
/* Текущее устройство воспроизведения звука.
Empty - устройство по умолчанию / без звука */
inline Unique<Device_name> cur_snd_dev {};
}
