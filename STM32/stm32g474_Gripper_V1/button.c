#include "button.h"
#include "swtimer.h"


#define BUTTON_EVENT_MAX          4



typedef struct
{
  bool        pressed;
  bool        pressed_event;
  uint16_t    pressed_cnt;
  uint32_t    pressed_start_time;
  uint32_t    pressed_end_time;

  bool        released;
  bool        released_event;
  uint32_t    released_start_time;
  uint32_t    released_end_time;

  bool        repeat_update;
  uint32_t    repeat_cnt;
  uint32_t    repeat_time_detect;
  uint32_t    repeat_time_delay;
  uint32_t    repeat_time;
} button_t;

// typedef struct
// {
//   GPIO_TypeDef *port;
//   uint32_t      pin;
//   GPIO_PinState on_state;
// } button_pin_t;


static void buttonISR(void *arg);
static bool buttonGetPin(uint8_t ch);


const button_pin_t button_pin[BUTTON_MAX_CH] =
{
    {GPIOB, GPIO_PIN_2,  GPIO_PIN_RESET},  // 0. KEY
};

const char *button_name[BUTTON_MAX_CH+1] = 
{
  "0_KEY",
};

static button_t button_tbl[BUTTON_MAX_CH];
static bool is_enable = true;
static uint16_t event_cnt = 0;
static uint8_t event_level = 5;
static button_event_t *event_tbl[BUTTON_EVENT_MAX];


bool buttonInit(void)
{
  bool ret = true;
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOB_CLK_ENABLE();


  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;

  for (int i=0; i<BUTTON_MAX_CH; i++)
  {
    GPIO_InitStruct.Pin = button_pin[i].pin;
    // if(i == 1) // Weact G431
    // {
    //   GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    // }
    // else
    // {
    //   GPIO_InitStruct.Pull = GPIO_PULLUP;
    // }
    
    HAL_GPIO_Init(button_pin[i].port, &GPIO_InitStruct);
    
  }

  for (int i=0; i<BUTTON_MAX_CH; i++)
  {
    button_tbl[i].pressed_cnt    = 0;
    button_tbl[i].pressed        = 0;
    button_tbl[i].released       = 0;
    button_tbl[i].released_event = 0;

    button_tbl[i].repeat_cnt     = 0;
    button_tbl[i].repeat_time_detect = 60;
    button_tbl[i].repeat_time_delay  = 250;
    button_tbl[i].repeat_time        = 200;

    button_tbl[i].repeat_update = false;
  }

  for (int i=0; i<BUTTON_EVENT_MAX; i++)
  {
    event_tbl[i] = NULL;
  }

  swtimer_handle_t timer_ch;
  timer_ch = swtimerGetHandle();
  if (timer_ch >= 0)
  {
    swtimerSet(timer_ch, 10, LOOP_TIME, buttonISR, NULL);
    swtimerStart(timer_ch);
  }
  else
  {
    // logPrintf("[NG] buttonInit()\r\n     swtimerGetHandle()\r\n");
  }

  return ret;
}

bool buttonEventInit(button_event_t *p_event, uint8_t level)
{
  bool ret = false;


  if (event_cnt < BUTTON_EVENT_MAX)
  {
    memset(p_event, 0, sizeof(button_event_t));

    event_tbl[event_cnt] = p_event;
    p_event->level = level;
    p_event->index = event_cnt;
    event_cnt++;

    p_event->is_init = true;
    ret = true;
  }

  return ret;
}

void buttonISR(void *arg)
{
  uint32_t repeat_time;


  for (int i=0; i<BUTTON_MAX_CH; i++)
  {
    if (buttonGetPin(i) == true)
    {
      if (button_tbl[i].pressed == false)
      {
        button_tbl[i].pressed_event = true;
        button_tbl[i].pressed_start_time = millis();

        for (int e_i=0; e_i<event_cnt; e_i++)
        {
          if (event_tbl[e_i]->level <= event_level)
            event_tbl[e_i]->pressed_event[i] = true;
          else
            event_tbl[e_i]->pressed_event[i] = false;
        }
      }

      button_tbl[i].pressed = true;
      button_tbl[i].pressed_cnt++;

      if (button_tbl[i].repeat_cnt == 0)
      {
        repeat_time = button_tbl[i].repeat_time_detect;
      }
      else if (button_tbl[i].repeat_cnt == 1)
      {
        repeat_time = button_tbl[i].repeat_time_delay;
      }
      else
      {
        repeat_time = button_tbl[i].repeat_time;
      }
      if (button_tbl[i].pressed_cnt >= repeat_time)
      {
        button_tbl[i].pressed_cnt = 0;
        button_tbl[i].repeat_cnt++;
        button_tbl[i].repeat_update = true;

        for (int e_i=0; e_i<event_cnt; e_i++)
        {
          if (event_tbl[e_i]->level <= event_level)
            event_tbl[e_i]->repeat_event[i] = true;
          else
            event_tbl[e_i]->repeat_event[i] = false;
        }        
      }

      button_tbl[i].pressed_end_time = millis();

      button_tbl[i].released = false;
    }
    else
    {
      if (button_tbl[i].pressed == true)
      {
        button_tbl[i].released_event = true;
        button_tbl[i].released_start_time = millis();

        for (int e_i=0; e_i<event_cnt; e_i++)
        {
          if (event_tbl[e_i]->level <= event_level)
            event_tbl[e_i]->released_event[i] = true;
          else
            event_tbl[e_i]->released_event[i] = false;
        }          
      }

      button_tbl[i].pressed  = false;
      button_tbl[i].released = true;
      button_tbl[i].repeat_cnt = 0;
      button_tbl[i].repeat_update = false;

      button_tbl[i].released_end_time = millis();
    }
  }
}

bool buttonGetPin(uint8_t ch)
{
  bool ret = false;

  if (ch >= BUTTON_MAX_CH)
  {
    return false;
  }

  if (HAL_GPIO_ReadPin(button_pin[ch].port, button_pin[ch].pin) == button_pin[ch].on_state)
  {
    ret = true;
  }

  return ret;
}

void buttonClear(void)
{
  for (int i=0; i<BUTTON_MAX_CH; i++)
  {
    button_tbl[i].pressed_start_time    = 0;
    button_tbl[i].pressed_end_time      = 0;
    button_tbl[i].released_start_time   = 0;
    button_tbl[i].released_end_time     = 0;

    button_tbl[i].pressed_event = false;
    button_tbl[i].released_event = false;
  }
}

void buttonEnable(bool enable)
{
  is_enable = enable;
}

bool buttonGetPressed(uint8_t ch)
{
  if (ch >= BUTTON_MAX_CH || is_enable == false)
  {
    return false;
  }

  return button_tbl[ch].pressed;
}

uint32_t buttonGetData(void)
{
  uint32_t ret = 0;


  for (int i=0; i<BUTTON_MAX_CH; i++)
  {
    ret |= (buttonGetPressed(i)<<i);
  }

  return ret;
}

const char *buttonGetName(uint8_t ch)
{
  ch = constrain(ch, 0, BUTTON_MAX_CH);

  return button_name[ch];
}

uint8_t  buttonGetPressedCount(void)
{
  uint32_t i;
  uint8_t ret = 0;

  for (i=0; i<BUTTON_MAX_CH; i++)
  {
    if (buttonGetPressed(i) == true)
    {
      ret++;
    }
  }

  return ret;
}

void buttonSetRepeatTime(uint8_t ch, uint32_t detect_ms, uint32_t repeat_delay_ms, uint32_t repeat_ms)
{
  if (ch >= BUTTON_MAX_CH || is_enable == false) return;

  button_tbl[ch].repeat_update = false;
  button_tbl[ch].repeat_cnt = 0;
  button_tbl[ch].pressed_cnt = 0;

  button_tbl[ch].repeat_time_detect = detect_ms;
  button_tbl[ch].repeat_time_delay  = repeat_delay_ms;
  button_tbl[ch].repeat_time        = repeat_ms;
}

uint32_t buttonGetRepeatCount(uint8_t ch)
{
  volatile uint32_t ret = 0;

  if (ch >= BUTTON_MAX_CH || is_enable == false) return 0;

  ret = button_tbl[ch].repeat_cnt;

  return ret;
}

uint32_t buttonGetPressedTime(uint8_t ch)
{
  volatile uint32_t ret;


  if (ch >= BUTTON_MAX_CH || is_enable == false) return 0;


  ret = button_tbl[ch].pressed_end_time - button_tbl[ch].pressed_start_time;

  return ret;
}


bool buttonGetReleased(uint8_t ch)
{
  bool ret;


  if (ch >= BUTTON_MAX_CH || is_enable == false) return false;

  ret = button_tbl[ch].released;

  return ret;
}

uint32_t buttonGetReleasedTime(uint8_t ch)
{
  volatile uint32_t ret;


  if (ch >= BUTTON_MAX_CH || is_enable == false) return 0;


  ret = button_tbl[ch].released_end_time - button_tbl[ch].released_start_time;

  return ret;
}

bool buttonEventClear(button_event_t *p_event)
{
  bool ret = true;


  if (is_enable == false) return false;
  if (p_event->is_init != true) return false;
  
  for (int i=0; i<BUTTON_MAX_CH; i++)
  {
    event_tbl[p_event->index]->released_event[i] = false;
  }
  return ret;
}

bool buttonEventGetReleased(button_event_t *p_event, uint8_t ch)
{
  bool ret;


  if (ch >= BUTTON_MAX_CH || is_enable == false) return false;
  if (p_event->is_init != true) return false;
  

  ret = event_tbl[p_event->index]->released_event[ch];
  event_tbl[p_event->index]->released_event[ch] = false;

  return ret;
}

bool buttonEventGetPressed(button_event_t *p_event, uint8_t ch)
{
  bool ret;


  if (ch >= BUTTON_MAX_CH || is_enable == false) return false;
  if (p_event->is_init != true) return false;
  

  ret = event_tbl[p_event->index]->pressed_event[ch];
  event_tbl[p_event->index]->pressed_event[ch] = false;

  return ret;
}

uint32_t buttonEventGetRepeat(button_event_t *p_event, uint8_t ch)
{
  volatile uint32_t ret = 0;

  if (ch >= BUTTON_MAX_CH || is_enable == false) return 0;
  if (p_event->is_init != true) return false;

  if (event_tbl[p_event->index]->repeat_event[ch])
  {
    event_tbl[p_event->index]->repeat_event[ch] = false;
    ret = button_tbl[ch].repeat_cnt;
  }

  return ret;
}

char* getButtonPort(button_pin_t button)
{
  int buttonPort = (int)button.port;
  char* targetName = "";

  switch (buttonPort)
  {
  case GPIOA_BASE:
    targetName = "GPIOA";
    break;
  case GPIOB_BASE:
    targetName = "GPIOB";
    break;
  case GPIOC_BASE:
    targetName = "GPIOC";
    break;
  case GPIOD_BASE:
    targetName = "GPIOD";
    break;
  case GPIOE_BASE:
    targetName = "GPIOE";
    break;
  case GPIOF_BASE:
    targetName = "GPIOF";
    break;
  default:
  targetName = "Unknown port";
    break;
  }

  return targetName;
}

uint8_t getButtonPin(uint32_t num)
{
  // int count = 0, quotient, remainder, i, digit;

  int arr[16] = {0,};
  int count = 0, quotient, remainder, digit;
  do
  {
    quotient = num/2;
    remainder = num - quotient * 2;
    arr[count++] = remainder;
    num = quotient;
  }while(quotient != 0);
  // cliPrintf(" %d \r\n", count);
  if(count % 4 == 0)
  {
    digit = count / 4;
  }
  else
  {
    digit = count / 4 + 1;
  }

  // // for (int i = digit * 4 - 1; i >= 0; i--)
  // for (int i = sizeof(arr)/sizeof(arr[0]) - 1; i >= 0; i--)
  // {
  //   if (i % 4 == 0)
  //   {
  //     cliPrintf("%d ", arr[i]);
  //   }
  //   else
  //   {
  //     cliPrintf("%d", arr[i]);
  //   }
  // }
  
  return count - 1;
}
