// Copyright 2011 Christoph Reiter
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
//

#include "stm3210c_eval_lcd.h"

#include <hactar/displays/stm3210c_eval.h>

static void init(DisplayInfo *display, size_t width, size_t height)
{
    DisplayInfoStm32cEval *eval_disp = (DisplayInfoStm32cEval*)display;

    // reset dirty markers
    eval_disp->dirty_y_max_ = display->height_ - 1;
    eval_disp->dirty_y_min_ = 0;

    LCD_Setup();
}

static void getPos(FbInfo *fb, size_t x, size_t y,
        size_t *index, uint8_t *offset)
{
    *index = (y / 8) * fb->width_ + x;
    *offset = y % 8;
}

static void update(DisplayInfo *display, FbInfo *fb)
{
    DisplayInfoStm32cEval *eval_disp = (DisplayInfoStm32cEval*)display;
    size_t x, y = eval_disp->dirty_y_min_;

    for(; y <= eval_disp->dirty_y_max_ && y < fb->height_; y++)
    {
        LCD_SetCursor(y, fb->display_->width_ - 1);
        LCD_WriteRAM_Prepare();
        for(x = 0; x < fb->width_; x++)
        {
            if(fbGetPixel(fb, x, y) == FB_BLACK)
                LCD_WriteRAM(LCD_COLOR_BLACK);
            else
                LCD_WriteRAM(LCD_COLOR_WHITE);
        }
        LCD_CtrlLinesWrite(LCD_NCS_GPIO_PORT, LCD_NCS_PIN, Bit_SET);
    }

    // reset dirty markers
    eval_disp->dirty_y_max_ = 0;
    eval_disp->dirty_y_min_ = fb->height_ - 1;
}

static void inval(DisplayInfo *display, size_t x, size_t y)
{
    DisplayInfoStm32cEval *eval_disp = (DisplayInfoStm32cEval*)display;

    if(y > eval_disp->dirty_y_max_)
        eval_disp->dirty_y_max_ = y;

    if(y < eval_disp->dirty_y_min_)
        eval_disp->dirty_y_min_ = y;
}

static void off(DisplayInfo *display)
{
    LCD_DisplayOff();
}

static void on(DisplayInfo *display)
{
    LCD_DisplayOn();
}

DisplayInfoStm32cEval stm32c_eval_display = {
    .base_ = {
        .init_ = &init,
        .get_pos_ = &getPos,
        .update_ = &update,
        .inval_ = &inval,
        .off_ = &off,
        .on_ = &on,
        .width_ = LCD_PIXEL_WIDTH,
        .height_ = LCD_PIXEL_HEIGHT,
    },
    .dirty_y_min_ = 0,
    .dirty_y_max_ = 0,
};