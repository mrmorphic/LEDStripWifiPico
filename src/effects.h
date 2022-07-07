#ifndef __LED__EFFECTS_H__
#define __LED__EFFECTS_H__

#include <stdio.h>

// The supported display modes.
typedef enum {
  AMBIENT_DRIFT,
  COLOUR,
  ROBOT,
  MATRIX, // TODO: green pattern that shifts
  ORB,    // TODO: single colour that pulses
  CYLON,
} DisplayMode;

typedef union {
  struct {
    uint32_t colour;
  } colorArgs;
} DisplayArgs;

typedef struct {
  DisplayMode mode;
  DisplayArgs arguments;
} Display;

typedef struct rgb_struct {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} rgb_struct;

void startRobotDisplay(void);
void startColourDisplay(uint32_t colour);
void startAmbientDisplay(void);
void startCylonDisplay(void);
void colourFromIndex(rgb_struct *rgb, int index, int pixel);

#endif