#include <AFMotor.h>
#include <SoftwareSerial.h>
#include "CSonar.h"
#include "CEcran.h"
#include "FonctionsSTD.h"
#include "config.h"


class CRobot
{
public:

  // constructeur de la classe CRobot
  CRobot(int pinBT_RX, int pinBT_TX, int pinSONAR_TRIG, int pinSONAR_ECHO, int distObstacle, int idMoteurG, int idMoteurD, int tempsOisifSecondes, int pinMP3_TX, int pinMP3_RX) : _sonar( pinSONAR_TRIG, pinSONAR_ECHO, distObstacle), _ecran(4), _moteurG(idMoteurG),_moteurD(idMoteurD)
  {
    // instanciation de l'objet SoftwareSerial qui sera notre sous objet Robot.bluetooth
    _btobj = new SoftwareSerial(pinBT_RX, pinBT_TX);
    // démarrage du port série bluetooth
    _btobj->begin(9600);
    // fixer le timeout pour les communications bluetooth
    _btobj->setTimeout(BT_READ_TIMEOUT_MS);
 
    // instanciation de l'objet CSonar
    //_sonar = new CSonar(pinSONAR_TRIG, pinSONAR_ECHO, distObstacle);
    //_ecran = new CEcran(OLED_RESET);
    // définition des moteurs, affectation de leur identifiant (entre 1 et 4)
    //_moteurG = new AF_DCMotor(idMoteurG);
    //_moteurD = new AF_DCMotor(idMoteurD);
    // init des variables temporelles
    _datedernierordre = millis();
    _tempsOisifSecondes = tempsOisifSecondes;
    _orientationcible = 0; // au départ on pointe vers le nord magnétique (on utilise un composant "compass" pour une position absolue)
  };
  // ici on traite les taches de fond, les états, et les actions à entreprendre ou à stopper.
  void Traitements()
  {
    // 
  }

// fonction qui modifie les états du robot selon la commande bluetooth reçue
void DemandeAction(String strAction)
{
  if(strAction="AVANCE") {};
  
  // on affiche à l'écran l'action reçue en Bluetooth
  _ecran.AfficherTexte("BT++"+strAction);
  delay(500);
}

  // cette fonction renvoie vrai/faux selon qu'il y a quelque chose à lire en réception bluetooth, et retourne la chaine dans strRecu (lue jusqu'au LF)
  bool Bluetooth_lecture(String& strRecu)
  {
    if(_btobj->available())
    {
      // si il y a des caractères, on lit & retourne la chaine jusqu'au prochain LF
      _ecran.AfficherTexte("BT ...");
      strRecu = _btobj->readStringUntil(10);
      _ecran.efface(true);
      return (true);
    }
    // si rien : on renvoie une chaine vide
    strRecu = "";
    return (false);
  };

  void AfficherTexte(String s)
  {
    _ecran.AfficherTexte(s);
    delay(200);
  }
  
  bool EstConnecte()
  {
    return (_btobj->available());
  }

  // ROBOT.ARRETER()
  void Arreter()
  {
    __Moteurs_Gauche(RELEASE,0);
    __Moteurs_Droite(RELEASE,0);
    _ecran.AfficherTexte("STOP");
  }
  
  // ROBOT.AVANCER()
  void Avancer()
  {
    // avancer signifie activer les 2 moteurs à la même vitesse
    __Moteurs_Gauche(FORWARD, MOTEUR_VITESSE);
    __Moteurs_Droite(FORWARD, MOTEUR_VITESSE);
  };

  // ROBOT.RECULER()
  void Reculer()
  {
    // reculer signifie activer les 2 moteurs à la même vitesse, en arrière
    __Moteurs_Gauche(BACKWARD, MOTEUR_VITESSE);
    __Moteurs_Droite(BACKWARD, MOTEUR_VITESSE);
  };

 // ROBOT.TOURNERENDEGRE()
 void TournerEnDegre(int angle)
 {
  // on ne fait que changer l'angle cible
   __nouvelangle(angle);
 }
  // ROBOT.TOURNERAGAUCHE()
  void TournerAGauche()
  {
    // on ne fait que changer l'angle cible
    __nouvelangle(-90);
  };

  // ROBOT.TOURNERADROITE()
  void TournerADroite()
  {
    // on ne fait que changer l'angle cible
    __nouvelangle(+90);
  };


  // fournir la vitesse courante en CM / SECONDE
  int Vitesse_Lire()
  {
    // @ TODO A PROGRAMMER DEPUIS L'ACCELEROMETRE
  };

  // détermine la nouvelle vitesse cible en CM / SECONDE
  int VitesseChanger(int vit)
  {
    _vitesse_cible_cm_sec = vit;
  };

  bool Obstacle()
  {
    _sonar.obstacle();
  };

  // renvoie vrai si le robot est passé en mode Autonome
  bool modeAutonome()
  {
    // on est en mode autonome si ca fait plus de tempsOisifSecondes qu'on a rien reçu en BT
    return ((_datedernierordre + _tempsOisifSecondes * 1000) < millis());

  };


private:

  // réinitialise le timer de réception bluetooth, trace du dernier ordre reçu
  void __ReinitTempsInactif()
  {
    _datedernierordre = millis();
  };

  void __Moteurs_Gauche(int action, int motorspeed = -1)
  {
    // argument optionel de redéfinition de la vitesse
    if (motorspeed > -1) _moteurG.setSpeed(motorspeed);

    // dans tous les cas on fait l'action demandée
    _moteurG.run(action);
  };

  // effectuer l'action demandée sur le moteur de droite
  void __Moteurs_Droite(int action, int motorspeed = -1)
  {
    // argument optionel de redéfinition de la vitesse
    if (motorspeed > -1) _moteurD.setSpeed(motorspeed);

    // dans tous les cas on fait l'action demandée
    _moteurD.run(action);
  };

  // déterminer le nouvel angle sur un arc de 360 degrés
  void __nouvelangle(int angleoffset)
  {
    // on s'assure de toujours être dans l'intervalle [0-360[ en sortie
    _orientationcible=((_orientationcible+angleoffset+360)%360);
  };
  
  SoftwareSerial* _btobj; // bluetooth
  CSonar _sonar; // sonar
  CEcran _ecran; // ecran (en fait 2 écrans, mais recevant les mêmes ordres)
  AF_DCMotor _moteurG; // moteur gauche
  AF_DCMotor _moteurD; // moteur droit
  unsigned long _datedernierordre; // derniere action bluetooth reçue
  int _tempsOisifSecondes; // param de délai avant passage en autonome
  int _orientationcible; // en degré l'orientation absolue par rapport au nord magnétique
  
  // vitesse cible (la vitesse instantannée se calcule uniquement via une fonction, et n'a pas de variable)
  int _vitesse_cible_cm_sec;

};
