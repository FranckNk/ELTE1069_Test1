/*

TITRE          : Control Servo Moteur avec Detecteur de Rotation. Test ELTE1069
AUTEUR         : Franck Nkeubou Awougang
DATE           : 09/02/2022
DESCRIPTION    : Programme pour le Test de ELTE permettant de controler le servo moteur 
					avec un detecteur de rotation.
VERSION        : 0.0.1

*/
#include <Arduino.h>
#include <Servo.h>
#include "WIFIConnector_MKR1000.h"
#include "MQTTConnector.h"

// Declaration des classes.
Servo ServoMoteur;  // Création d'un objet de type Servo (un servo moteur).

// Declaration des constantes.
const int PIN_SERVO_MOTEUR = 9; // Pin attachee au servo moteur
const int PIN_CAPTEUR_ROTATION = A0;  // Pin attachee au capteur de rotation.

// Declaration des variables.
unsigned short int DelayAttente = 15; 		// Temp d'attente pour que le servo moteur se positionne.
unsigned short int ValueAngleServo = 0;        // Valeur envoyee dans le servo moteur.
unsigned short int RotationSensorValue = 0;        // Valeur lue par le capteur de roation.
unsigned short int LastValueAngleServo = 0; 		// Derniere valeur de l'angle du servo moteur.

void setup() {
	//Initialisation du wifi.
	wifiConnect();                  //Branchement au réseau WIFI
	MQTTConnect();                  //Branchement au broker MQTT
	// initialisation du moniteur serie.
	Serial.begin(9600);
	ServoMoteur.attach(PIN_SERVO_MOTEUR);  // Spécifie la ServoPIN à notre servo moteur.
	LastValueAngleServo = ServoMoteur.read(); // On specifie la derniere valeur a l'angle actuel.
	// ServoMoteur.write(0); // On reset la valeur de l'angle au debut.
	// delay(2000); // On attend deux secondes pour reset.
}

void loop() {
	// Lecture de la valeur du capteur de rotation.
	RotationSensorValue = analogRead(PIN_CAPTEUR_ROTATION);
	// Faisons un mapping pour avoir la correspondance suivant la limite du control de l'angle.
	ValueAngleServo = map(RotationSensorValue, 0, 1023, 0, 90);
	// Changeons la valeur du servo moteur avec la nouvelle valeur obtenue.
	ServoMoteur.write(ValueAngleServo);  
	delay(DelayAttente); // Attendons que le servo moteur se positionne.
	// condition d'envoi des donnees sur Thingsboard. On envoi uniquement lorsqu'on observe un changement de deux degres.
	if (ValueAngleServo - LastValueAngleServo == 3 || ValueAngleServo - LastValueAngleServo == -3){
		// On a obtenu une variation de 02 degres soit de plus ou soit de moins.
		
		Serial.print("\n\nVariation OBTENUE.\n\n = ");
		// On prepare la chaine a envoyer sur la plateforme IdO.
		appendPayload("ValueAngleServo", ValueAngleServo);
		appendPayload("RotationSensorValue", RotationSensorValue);
		// On envoi les informations sur la plateforme IdO.
		sendPayload();
		LastValueAngleServo = ValueAngleServo; // On sauvegarde la derniere variation.
	}
	// // print the results to the serial monitor:
	// Serial.print("sensor = ");
	// Serial.print(RotationSensorValue);
	// Serial.print("\t output = ");
	// Serial.println(ValueAngleServo);

	// // wait 2 milliseconds before the next loop
	// // for the analog-to-digital converter to settle
	// // after the last reading:
}