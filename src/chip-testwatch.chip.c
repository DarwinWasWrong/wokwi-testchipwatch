
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

  chip->pin_ENA = pin_init("EN A",INPUT);
  chip->pin_ENB = pin_init("EN B",INPUT);
  chip->pin_IN1 = pin_init("IN1",INPUT);
  chip->pin_IN2 = pin_init("IN2",INPUT);
  chip->pin_IN3 = pin_init("IN3",INPUT);
  chip->pin_IN4 = pin_init("IN4",INPUT);

  printf( " %s\n","Chip Init started");

  // pwm timings
  unsigned long high_time_ENA;
  unsigned long low_time_ENA;
  unsigned long high_time_ENB;
  unsigned long low_time_ENB;
  
  // Display values
  chip->speed_percent_A=0;
  chip->speed_percent_B=0;
  // dummy value to fire of percentage bar draw on start
  chip->previous_speed_percent_A= 10;
  chip->previous_speed_percent_B= 10;

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

   // PWM watches
  pin_watch(chip->pin_ENA, &watch_config_a);
  pin_watch(chip->pin_ENB, &watch_config_b);
  

  // pins watches
  pin_watch(chip->pin_IN1, &watch_config);
  pin_watch(chip->pin_IN2, &watch_config);
  pin_watch(chip->pin_IN3, &watch_config);
  pin_watch(chip->pin_IN4, &watch_config);


}

// PWM A pin change function for watch
void chip_pin_change_PWM_A(void *user_data, pin_t pin, uint32_t value) {
  chip_state_t *chip = (chip_state_t*)user_data;
  uint8_t ENA = pin_read(chip->pin_ENA);
   printf( " %s\n","------ Pin Change A");
// channel A using PWM
 
  if (ENA == 1){
    chip->high_ENA = get_sim_nanos();
    chip->low_time_ENA = chip->high_ENA - chip->low_ENA;
  } else {
    chip->low_ENA = get_sim_nanos();
    chip->high_time_ENA = chip->low_ENA - chip->high_ENA ;
  }
  float total_ENA = chip->high_time_ENA + chip->low_time_ENA;
  int duty_cycle_ENA = (chip->high_time_ENA / total_ENA) * 100.0;
  chip->speed_percent_A=duty_cycle_ENA;
 
}

// PWM B pin change function for watch
void chip_pinb_change(void *user_data, pin_t pin, uint32_t value) {
  
  chip_state_t *chip = (chip_state_t*)user_data;
  uint8_t ENB = pin_read(chip->pin_ENB);
  printf( " %s\n","----------------   Pin Change B");

  if (ENB){
    chip->high_ENB= get_sim_nanos();
    chip->low_time_ENB= chip->high_ENB- chip->low_ENB;
  } else {
    chip->low_ENB= get_sim_nanos();
    chip->high_time_ENB= chip->low_ENB- chip->high_ENB;
  }
  float total = chip->high_time_ENB+ chip->low_time_ENB;
  int duty_cycle_ENB = (chip->high_time_ENB / total) * 100.0;
  chip->speed_percent_B=duty_cycle_ENB;
 


}


void chip_pin_change(void *user_data, pin_t pin, uint32_t value) {
  chip_state_t *chip = (chip_state_t*)user_data;
   printf( " %s\n","on IN pins");
  //uint8_t ENA = pin_read(chip->pin_ENA);
  //uint8_t ENB = pin_read(chip->pin_ENA);
  uint8_t IN1 = pin_read(chip->pin_IN1);
  uint8_t IN2 = pin_read(chip->pin_IN2);
  uint8_t IN3 = pin_read(chip->pin_IN3);
  uint8_t IN4 = pin_read(chip->pin_IN4);

  printf( " %s\n","on IN pins");

}

