#ifndef PARAMS_H
#define PARAMS_H

#include <math.h>

// sampling time
unsigned long lDt = 100*1e3;

//filter size
const int filter_size = 11;

// Utilisateur
bool Etat_dir = false; //false = Masquer cote interieur de la bobine, True = masquer cote exterieur de la bobine
double Vitesse_des = 40; // cm/min - Vitesse_max = 75 cm/min
double Tension_des = 40; // Tension entre 0 et 100

// Speed- and Torque Motor params
double des_pos = Tension_des/100*14+8; // Convertion de la tension en position entre 8 mm et 22 mm
double rest_pos = des_pos; // max 25 mm
double dia_encoder = 6.0; //cm
//double retard_temps = 0.9658; //ratio : temps_sys = ratio * temps_reel
double retard_temps = 1.0; //ratio : temps_sys = ratio * temps_reel
const int MAX_SPEED = 5; // Le moteur a une vitesse max de 500 tours/min, soit 5 tours/min avec la gear box (gear_ratio*MAX_speed = 500)
const int gear_ratio = 100;
const int MAX_TORQUE = 100; 
double moving_speed = Vitesse_des / (dia_encoder*M_PI*retard_temps);


// Autre (pas necessaire)
double radius_ratio = 30.0/60.0; // rayon bobine encodeur / rayon du moteur 
double motor_speed = moving_speed*radius_ratio*gear_ratio;



#endif 