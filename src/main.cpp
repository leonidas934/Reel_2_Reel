#include <Arduino.h>
#include <TimerOne.h>

#include "state_machine.h"
#include "board.h"
#include "pin.h"
#include "mv_average_filter.h"
#include "PID.h"
#include "speed_motor.h"
#include "brake_motor.h"
#include "params.h"


Board _board;
Speed_motor speed_motor(_board, MAX_SPEED*gear_ratio);
Breake_motor breake_motor(_board, MAX_TORQUE);

state_machine kernel(_board);
PID pos_controller(0,0.01,0,0, 90); // 90% de torque max
//PID speed_controller(38,0.0271,35470,0, 450); // PID pour Vitesse_des = 50, 450 = 90% de MAX_speed
//PID speed_controller(30,0.0065,15200,0, 450); // PID pour vitesse_des = 10
double Kp_des = (Vitesse_des-10)/65*8+25;
double Ki_des = (Vitesse_des-10)/65*0.021+0.007;
double Kd_des = (Vitesse_des-10)/65*20270+13000;
PID speed_controller(Kp_des,Ki_des,Kd_des,0, 100); //Approx lineaire des deux PID

mv_average_filter speed_filter(filter_size);
double test = 0.0;
unsigned long lastPrintTime = 0;
const unsigned long printInterval = 50;  // Print every nb ms

void encoder_step() {
 _board._encoder.step();
}

void change_state(){
  kernel.change_state(); 
  speed_motor.init();
    breake_motor.init();
    pos_controller.innit();
    speed_controller.innit();
    _board.init();
    speed_filter.init();
}

void motor_feedback(){
  // speed_motor.measure_feedback();
}

void change_direction(){
  if (kernel.state != off)
  {
  speed_motor.change_direction();
  }
}

void print_results(double time, double moving_speed, double speed_smooth, double input_speed, double input_breake, double des_pos, double pos, double test){
    Serial.print(time/1000, 4);
    Serial.print(" ,");
    Serial.print(moving_speed, 4);
    Serial.print(" ,");
    Serial.print(speed_smooth, 4);
    Serial.print(" ,");
    Serial.print(double(input_speed)/ICR4 *100, 4);
    Serial.print(" ,");
    Serial.print(double(input_breake)/ICR4 *100, 4);
    Serial.print(" ,");
    Serial.print(des_pos, 4);
    Serial.print(" ,");
    Serial.print(pos, 4);
    Serial.print(" ,");
    Serial.println(test, 4);
}




 void main_loop(){
  switch (kernel.state)
  {
  case off:{
    digitalWrite(led_pin, LOW);
    speed_motor.enable();
    breake_motor.enable();

    break;

  }
    


  case on:{
    digitalWrite(led_pin, HIGH);
    double time = kernel.get_time();

    double breake_des_start_pos = pos_controller.output(rest_pos, _board._pos_sensor.get_pos(), time);
    int input_breake_start_pos = breake_motor.get_dc(breake_des_start_pos);
    breake_motor.set(int(input_breake_start_pos));
    
    unsigned long now = millis();
    if (now - lastPrintTime >= printInterval ) { //print intervall = nb ms
      lastPrintTime = now;
      print_results(millis()-kernel.start_time, 0, 0, 0, input_breake_start_pos, rest_pos, _board._pos_sensor.get_pos(), test);
    }
    break;
  }
    

  case running:{
    double time = kernel.get_time();
    double speed =_board._encoder.get_speed();
    speed_filter.push(speed);
    double speed_smooth = speed_filter.get_average();
    
    double pos = _board._pos_sensor.get_pos();

    double breake_des = pos_controller.output(des_pos, pos, time);
    double speed_des = speed_controller.output(moving_speed, speed_smooth, time);
    double test = time;
    int input_speed = speed_motor.get_dc(speed_des);
    int input_breake = breake_motor.get_dc(breake_des);
    speed_motor.set(int(input_speed));
    breake_motor.set(int(input_breake));

    unsigned long now = millis();
    if (now - lastPrintTime >= printInterval ) { //print intervall = nb ms
      lastPrintTime = now;
      print_results(millis()-kernel.start_time, moving_speed, speed_smooth, input_speed, input_breake, des_pos, pos, test);
    }
    break;
    }
  }
 
  
};


void setup() {
Serial.begin(115200);
Timer1.initialize(lDt); 
Timer1.attachInterrupt(main_loop);

_board.init();
kernel.change_state();

attachInterrupt(digitalPinToInterrupt(encoderPinA), encoder_step, RISING);
attachInterrupt(digitalPinToInterrupt(on_off_pin), change_state, CHANGE);
attachInterrupt(digitalPinToInterrupt(start_motor_pin), change_state, CHANGE);
attachInterrupt(digitalPinToInterrupt(dir_pin), change_direction, CHANGE);

}
 
 



void loop() {

}
