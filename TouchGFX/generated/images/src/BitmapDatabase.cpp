// 4.20.0 0xed1715a3
// Generated by imageconverter. Please, do not edit!

#include <BitmapDatabase.hpp>
#include <touchgfx/Bitmap.hpp>

extern const unsigned char image_dark_icons_pause_32[]; // BITMAP_DARK_ICONS_PAUSE_32_ID = 0, Size: 26x32 pixels
extern const unsigned char image_dark_icons_play_32[]; // BITMAP_DARK_ICONS_PLAY_32_ID = 1, Size: 23x32 pixels

const touchgfx::Bitmap::BitmapData bitmap_database[] = {
    { image_dark_icons_pause_32, 0, 26, 32, 16, 2, 9, ((uint8_t)touchgfx::Bitmap::ARGB8888) >> 3, 26, ((uint8_t)touchgfx::Bitmap::ARGB8888) & 0x7 },
    { image_dark_icons_play_32, 0, 23, 32, 1, 8, 13, ((uint8_t)touchgfx::Bitmap::ARGB8888) >> 3, 14, ((uint8_t)touchgfx::Bitmap::ARGB8888) & 0x7 }
};

namespace BitmapDatabase
{
const touchgfx::Bitmap::BitmapData* getInstance()
{
    return bitmap_database;
}

uint16_t getInstanceSize()
{
    return (uint16_t)(sizeof(bitmap_database) / sizeof(touchgfx::Bitmap::BitmapData));
}
} // namespace BitmapDatabase
