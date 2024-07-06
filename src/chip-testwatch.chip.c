
#include "wokwi-api.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {

 //Module Pins
  pin_t pin_IN1;
  pin_t pin_IN2;
  pin_t pin_IN3;
  pin_t pin_IN4;

  pin_t pin_ENA;
  pin_t pin_ENB;

  unsigned long high_ENA;
  unsigned long low_ENA;
  unsigned long high_time_ENA;
  unsigned long low_time_ENA;

  unsigned long high_ENB;
  unsigned long low_ENB;
  unsigned long high_time_ENB;
  unsigned long low_time_ENB;
  
  uint8_t  speed_percent_A;
  uint8_t  speed_percent_B;
  uint8_t  previous_speed_percent_A;
  uint8_t  previous_speed_percent_B;
} chip_state_t;



// pin change watches
static void chip_pin_change(void *user_data, pin_t pin, uint32_t value);
static void chip_pin_change_PWM_A(void *user_data, pin_t pin, uint32_t value);
static void chip_pinb_change(void *user_data, pin_t pin, uint32_t value);

void chip_init(void) {
  chip_state_t *chip = malloc(sizeof(chip_state_t));

  chip->pin_ENA = pin_init("ENA",INPUT);
  chip->pin_ENB = pin_init("ENB",INPUT);
  chip->pin_IN1 = pin_init("IN1",INPUT);
  chip->pin_IN2 = pin_init("IN2",INPUT);
  chip->pin_IN3 = pin_init("IN3",INPUT);
  chip->pin_IN4 = pin_init("IN4",INPUT);

  printf( " %s\n","Chip Init started");

// config for PWM A watch
const pin_watch_config_t watch_config_a= {
    .edge = BOTH,
    .pin_change = chip_pin_change_PWM_A,
    .user_data = chip
  };

// config for PWM B watch
const pin_watch_config_t watch_config_b= {
    .edge = BOTH,
    .pin_change = chip_pinb_change,
    .user_data = chip
  };
  
  // config for other pins IN1 IN2 IN3 IN4
  const pin_watch_config_t watch_config = {
    .edge = BOTH,
    .pin_change = chip_pin_change,
    .user_data = chip
  };

   

  // pins watches
  pin_watch(chip->pin_IN1, &watch_config);
  pin_watch(chip->pin_IN2, &watch_config);
  pin_watch(chip->pin_IN3, &watch_config);
  pin_watch(chip->pin_IN4, &watch_config);

// PWM watches
   pin_watch(chip->pin_ENA, &watch_config_a);
  pin_watch(chip->pin_ENB, &watch_config_b);
}

// PWM A pin change function for watch
void chip_pin_change_PWM_A(void *user_data, pin_t pin, uint32_t value) {
  chip_state_t *chip = (chip_state_t*)user_data;
    printf( " %s\n","on ENA pin ");
}

// PWM B pin change function for watch
void chip_pinb_change(void *user_data, pin_t pin, uint32_t value) {
  chip_state_t *chip = (chip_state_t*)user_data;
    printf( " %s\n","on ENB pin");
}


void chip_pin_change(void *user_data, pin_t pin, uint32_t value) {
  chip_state_t *chip = (chip_state_t*)user_data;
   printf( " %s\n","on IN pins");
 }

