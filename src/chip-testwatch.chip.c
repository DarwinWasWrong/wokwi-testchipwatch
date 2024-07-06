
#include "wokwi-api.h"
#include <stdio.h>
#include <stdlib.h>

// thanks to
// Maverick - for saving my mind with PWM
//
// https://notisrac.github.io/FileToCArray/
// for the conversion for images
//
// to be added
// catch 0 and 255 in PWM




// the various states the channel can be in
const char drive_state[][17]=
{
  "Backward       ",
  "Forward        ", 
  "Brake Stop      ",
  "Free Stop      "
  };

// basic RGBA color
typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
} rgba_t;



typedef struct {
  
  //pin_t pin_out1;
  //pin_t pin_out2;
  //pin_t pin_out3;
  //pin_t pin_out4;

  //Module Pins
  pin_t pin_IN1;
  pin_t pin_IN2;
  pin_t pin_IN3;
  pin_t pin_IN4;
  pin_t pin_ENA;
  pin_t pin_ENB;

  // start of timing checks for the PWM
  bool use_PWM_ENA; //  pwm used
  bool use_PWM_ENB; //  pwm used

  unsigned long high_ENA;
  unsigned long low_ENA;
  unsigned long high_time_ENA;
  unsigned long low_time_ENA;

  unsigned long high_ENB;
  unsigned long low_ENB;
  unsigned long high_time_ENB;
  unsigned long low_time_ENB;

  uint32_t Vs_attr;  // power

  uint32_t fb_w;
  uint32_t fb_h;
  uint32_t row;
  buffer_t framebuffer;


  rgba_t   white;
  rgba_t   green;
  rgba_t   background;
  rgba_t   purple;
  rgba_t   black;
  rgba_t   red;
  rgba_t   blue;

 // text start and postion
  uint32_t vertical_start;
  uint32_t position_x;
  uint32_t position_y;
  
  uint8_t  speed_percent_A;
  uint8_t  speed_percent_B;
  uint8_t  previous_speed_percent_A;
  uint8_t  previous_speed_percent_B;

  uint8_t   drive_A_state;
  uint8_t   drive_B_state;
  uint8_t  previous_drive_A_state;
  uint8_t  previous_drive_B_state;
  // motor graphics position
  uint8_t motorApos;
  uint8_t motorBpos;
  uint8_t motor_A_y;
  uint8_t motor_A_x;
  uint8_t motor_2_x;
  uint8_t motor_B_x;

// arrow graphics position
  uint8_t motor_1_2_arrow_y ;

  uint8_t motorA_right_arrow_x;
  uint8_t motorA_left_arrow_x;

  uint8_t motorB_right_arrow_x;
  uint8_t motorB_left_arrow_x;

  uint8_t bar_1_2_y;
  uint8_t bar_left_x;
  uint8_t bar_right_x;


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

  chip->Vs_attr = attr_init_float("Vs", 12.0);



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

  // display colors
  chip-> white      = (rgba_t) { .r = 0xff, .g = 0xff, .b = 0xff, .a = 0xff };
  chip-> green      = (rgba_t) { .r = 0x08, .g = 0x7f, .b = 0x45, .a = 0xff };
  chip-> background = (rgba_t) { .r = 0xf7, .g = 0xf7, .b = 0xf7, .a = 0xff };
  chip-> purple    = (rgba_t) { .r = 0xff, .g=0x00,   .b=0xff,   .a=0xff   };
  chip-> black    =  (rgba_t) { .r = 0x00, .g=0x00,   .b=0x00,   .a=0x00   };
  chip-> red    =  (rgba_t) { .r = 0xff, .g=0x00,   .b=0x00,   .a=0x00   };
  chip-> blue    =  (rgba_t) { .r = 0x00, .g=0x00,   .b=0xff,   .a=0x00   };
  chip->row = 0;
  // get the screen size
  chip->framebuffer = framebuffer_init(&chip->fb_w, &chip->fb_h);
  printf("Framebuffer: fb_w=%d, fb_h=%d\n", chip->fb_w, chip->fb_h);
 
 // settings for gears and displays
 // phase of gear display
  chip->motorApos=0;
  chip->motorBpos=0;
  // positioning
  chip->motor_A_y=20;
  chip->motor_A_x=113;
  chip->motor_2_x=20;
  chip->motor_B_x=187;
  // arrow and speed base position
  chip->motor_1_2_arrow_y = 70;
  // Motor A arrow positions  relative to motor A position
  chip->motorA_right_arrow_x =chip->motor_A_x + 40;
  chip->motorA_left_arrow_x = chip->motor_A_x  ;
  // Motor B arrow positions  relative to motor B position
  chip->motorB_right_arrow_x = chip->motor_B_x +  40;
  chip->motorB_left_arrow_x = chip->motor_B_x;
  // bars position relative to motor
  chip-> bar_1_2_y=  chip->motor_1_2_arrow_y + 20;
  chip-> bar_left_x=chip->motor_A_x ;
  chip-> bar_right_x=chip->motor_B_x ;


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
  uint8_t ENA = pin_read(chip->pin_ENA);
  uint8_t ENB = pin_read(chip->pin_ENA);
  uint8_t IN1 = pin_read(chip->pin_IN1);
  uint8_t IN2 = pin_read(chip->pin_IN2);
  uint8_t IN3 = pin_read(chip->pin_IN3);
  uint8_t IN4 = pin_read(chip->pin_IN4);

  printf( " %s\n","on IN pins");




printf( "   chip->speed_percent_A %d chip->speed_percent_B  %d\n",chip->speed_percent_A,chip->speed_percent_B);
printf( "   previous_speed_percent_A  %d previous_speed_percent_B   %d\n",chip->previous_speed_percent_A,chip->previous_speed_percent_B);


}

