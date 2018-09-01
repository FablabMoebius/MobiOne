// mindmap du projet : https://www.mindmup.com/#m:g10BzengBiwNDtzd25SYW9vY0VTNlU

// ********************************************************************
// *** I N C L U D E S ************************************************
// ********************************************************************

#include "Arduino.h" // pour les éléments de base de l'arduino
#include "config.h" // pour la définition des constantes (numéros de broches, reglages ...)
#include <SoftwareSerial.h> // pour la communication des éléments série (bluetooth, mp3, ...)
#include <AFMotor.h> // pour la gestion du shield 'Adafruit Motor Shield V1'
#include "CRobot.h" // classe de notre robot
#include "FonctionsSTD.h" // fonctions génériques pratiques à (ré)utiliser
#include <DFPlayer_Mini_Mp3.h> /// pour le module MP3 de type DFPlayer

// ********************************************************************
// *** V A R I A B L E S **********************************************
// ********************************************************************

// instanciation de notre objet 'MOBEE' de classe CRobot, avec les constantes définies dans 'config.h' ci-dessus
CRobot MOBEE(PIN_BT_RX, PIN_BT_TX, PIN_SONAR_TRIG, PIN_SONAR_ECHO, SONAR_OBSTACLE,MOTEUR_ID_GAUCHE, MOTEUR_ID_DROIT, ROBOT_TEMPS_OISIF, PIN_MP3_RX, PIN_MP3_TX);

int counter=0 ;

// notre chaine temporaire de réception bluetooth
String strBTRecu=""; 

void setup()
{
  // initialisation du port série pour le PC
  Serial.begin(9600);
}

void loop()
{

  if (MODETEST==1)
  {
    MOBEE.AfficherTexte("MODE TEST");
    delay(1000);
    MOBEE.AfficherTexte("TEST MODE");
    delay(1000);
  }
  else
  {
  
    // un compteur générique qui nous permettra d'avoir des comportements tous les x passages (exemple 1 fois 2 ou 1 fois sur 3)
    counter+=1;
  
  
    if (MOBEE.EstConnecte())
    {
      // si une chaine est reçue via le Bluetooth, on affiche sur la console Série ...
      if(MOBEE.Bluetooth_lecture(strBTRecu)) Serial.println("BT <<" + strBTRecu + ">>");
    
      // on met à jour les états en fonction de la commande recue
      MOBEE.DemandeAction(strBTRecu);
    
      // on fait les traitements de fond en fonction des états du Robot
      MOBEE.Traitements();
    }
    else
    {
      // alterne "ATTENTE" et "BLUETOOTH" selon la parité de counter (on utilise l'opérateur conditionnel ternaire)
      MOBEE.AfficherTexte((counter%0==0)?"ATTENTE":"BLUETOOTH");
      delay(700);
    };
    
   
    // on attend un peu avant de boucler
    delay(300);
  };
}

