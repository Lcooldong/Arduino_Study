#ifndef BUTTON_H_
#define BUTTON_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <Arduino.h>


#define BUTTON_MAX_CH       1



typedef struct
{
  bool is_init;
  uint8_t level;
  uint8_t index;

  bool pressed_event[BUTTON_MAX_CH];
  bool released_event[BUTTON_MAX_CH];
  bool repeat_event[BUTTON_MAX_CH];
} button_event_t;

typedef struct
{
  GPIO_TypeDef *port;
  uint32_t      pin;
  GPIO_PinState on_state;
} button_pin_t;


bool     buttonInit(void);
bool     buttonGetPressed(uint8_t ch);
uint32_t buttonGetData(void);

uint32_t buttonGetPressedTime(uint8_t ch);
uint8_t  buttonGetPressedCount(void);

bool     buttonGetReleased(uint8_t ch);
uint32_t buttonGetReleasedTime(uint8_t ch);

void     buttonSetRepeatTime(uint8_t ch, uint32_t detect_ms, uint32_t repeat_delay_ms, uint32_t repeat_ms);
uint32_t buttonGetRepeatCount(uint8_t ch);

const char *buttonGetName(uint8_t ch);

bool     buttonEventInit(button_event_t *p_event, uint8_t level);
bool     buttonEventClear(button_event_t *p_event);
bool     buttonEventGetPressed(button_event_t *p_event, uint8_t ch);
bool     buttonEventGetReleased(button_event_t *p_event, uint8_t ch);
uint32_t buttonEventGetRepeat(button_event_t *p_event, uint8_t ch);

char*    getButtonPort(button_pin_t button);
uint8_t  getButtonPin(uint32_t num);

#endif

#ifdef __cplusplus
}
#endif



