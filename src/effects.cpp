#include <Arduino.h>
#include <stdlib.h>
#include "effects.h"

Display currentDisplay;

void startRobotDisplay(void) {
    currentDisplay.mode = ROBOT;
}

void startColourDisplay(uint32_t colour) {
    currentDisplay.mode = COLOUR;
    currentDisplay.arguments.colorArgs.colour = colour;
}

void startAmbientDisplay(void) {
    currentDisplay.mode = AMBIENT_DRIFT;
}

void startCylonDisplay(void) {
    currentDisplay.mode = CYLON;
}

void ambientDrift(rgb_struct *rgb, int index, int pixel) {
    if (index >= 1024) {
      index -= 2 * (index - 1024 + 1) - 1;
    }
    if (index < 256) {
      rgb->red = 0;
      rgb->green = index;
      rgb->blue = 255;
    } else if (index < 512) {
      rgb->red = 0;
      rgb->green = 255;
      rgb->blue = 511 - index;
    } else if (index < 768) {
      rgb->red = index - 512;
      rgb->green = 255;
      rgb->blue = 0;
    } else {
      rgb->red = 255;
      rgb->green = 1023 - index;
      rgb->blue = 0;
    }
  delay(25);
}

void fixedColour(rgb_struct *rgb, int index, int pixel) {
  uint32_t colour = currentDisplay.arguments.colorArgs.colour;

  rgb->red = colour >> 16 & 0xff;
  rgb->green = colour >> 8 & 0xff;
  rgb->blue = colour && 0xff;
}

void robot(rgb_struct *rgb, int index, int pixel) {
  // if (index != 0) {
  //   return;
  // }
  rgb->red = random(2) << 6;
  rgb->green = random(2) << 6;
  rgb->blue = random(2) << 6;
  delay(16);
  // pixels.setPixelColor(i, pixels.Color(random(4) << 5, random(4) << 5, random(4) << 5));
}

#define CYLON_MIN 3
#define CYLON_MAX (49 - CYLON_MIN)

int cylon_eye_center = CYLON_MIN;
int cylon_dir = 1;

void cylon(rgb_struct *rgb, int index, int pixel) {
    rgb->green = 0;
    rgb->blue = 0;
    int diff = abs(pixel - cylon_eye_center);
    if (diff <= 1) {
        rgb->red = 255;
    } else if (diff <= 2) {
        rgb->red = 191;
    } else if (diff <= 3) {
        rgb->red = 95;
    } else {
        rgb->red = 32;
    }

    // if (index >> 16 == 0) {
        cylon_eye_center += cylon_dir;
        if (cylon_eye_center < CYLON_MIN) {
            cylon_eye_center = CYLON_MIN;
            cylon_dir = 1;
        } else if (cylon_eye_center > CYLON_MAX) {
            cylon_eye_center = CYLON_MAX;
            cylon_dir = -1;
        }
    // }
}

// Generate a colour for nominated pixel in the sequence, given an
// index from 0-2047 that cycles, and the mode:
//   rgb    where to write the RGB value for this LED
//   index  value from 0 to 2047 that is constantly cycled through
//   pixel  the n-th pixel in the LED sequence, with 0 being first.
//   mode   how the pixel is calculated. One of MODE_* values.
void colourFromIndex(rgb_struct *rgb, int index, int pixel) {
  switch (currentDisplay.mode) {
    case AMBIENT_DRIFT:
      ambientDrift(rgb, index, pixel);
      break;
    case COLOUR:
      fixedColour(rgb, index, pixel);
      break;
    case ROBOT:
      robot(rgb, index, pixel);
      break;
    case MATRIX:
    case ORB:
      break;
    case CYLON:
        cylon(rgb, index, pixel);
  }
}
