
// pour la communication série (BT et MP3 player notamment)
#include <SoftwareSerial.h>
#include <SPI.h>
// pour la communication I2C (avec les ports SDA & SCL)
#include <Wire.h>
// pour les afficheurs OLED
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// pour la gestion du shield moteurs V1 de type Adafruit
#include <AFMotor.h>
// pour le lecteur mp3 DFPlayer
#include <DFPlayer_Mini_Mp3.h>

// activation des différentes parties :
#define ACTIVATE_BLUETOOTH 1 // présence du module bluetooth
#define ACTIVATE_OLED 1 // présence des écrans OLED
#define ACTIVATE_MOTORS 1 // présence du le shield moteur
#define ACTIVATE_SONAR 0 // présence du sonar
#define ACTIVATE_GYRO 0 // présence du module gyroscope
#define ACTIVATE_MP3PLAYER 0 // présence du module de lecture mp3

// BROCHAGE ET CONSTANTES
#define PIN_LED            13 // LED INTERNE
// ... pour le sonar
#define PIN_SONAR_TRIG     55 //A1
#define PIN_SONAR_ECHO     56 //A2
#define SONAR_DISTANCE_OBSTACLE_EN_CM 20 // distance à laquelle le robot s'arrête des obstacles

// ... pour le bluetooth
#define PIN_BLUETOOTH_RX    53
#define PIN_BLUETOOTH_TX    52
#define BT_READ_TIMEOUT_MS 2000 // nombre de millisec. avant abandon de lecture réception bluetooth

// ... pour le player mp3
#define PIN_MP3_RX  57
#define PIN_MP3_TX  58

// ... pour les écrans OLED
#define OLED_RESET       4
#define OLED_DEF_VITESSE_YEUX 7 // rapport fluidité <=> vitesse d'animation
#define OLED_INTERVALLE_CLIGNEMENT_OEIL_MS 6000

// ... pour les MOTEURS (attention, à trop diminuer cette valeur, les roues peuvent ne plus bouger, par manque de courant)
#define MOTEUR_VITESSE 200

// pour le GYROSCOPE (seul 1 axe sera utilisé Y ? dans le but de déterminer l'angle lors des rotations sur place)
// (inspiré du code simple trouvé ici : http://playground.arduino.cc/Main/Gyro)
#define PIN_GYROSCOPE  53 // A8 // entrée analogique pour lecture du gyroscope sur l'axe choisi
#define GYRO_VOLTAGE 5 // l'alimentation 3 ou 5V détermine la sensibilité
#define GYRO_ZEROVOLTAGE  2.375 // le niveau zéro (voir datasheet du composant utilisé), VDD min ou VNULL
#define GYRO_SENSITIVITY .007 // voir datasheet aussi pour la sensibilité en mV/deg/sec 
#define GYRO_ROTATIONTHRESHOLD 1; // seuil minimum de détection de rotation en degré par seconde

#define GYRO_TIMEOUT_MS 15000 // temps avant sortie de boucle de rotation moteur

// ... autres constantes
#define LF          10
#define ROBOT_TEMPS_OISIF 10 // combien de temps en seconde avant de passer en mode autonome.
#define ROBOT_PROBA_ACTION_AUTONOME 60 // coefficient (dépend de tous les delais internes, et donc n'a pas de base absolue de calcul, y aller à taton)

// marqueurs temporels
unsigned long  BT_DerniereDateReception;
unsigned long  OLED_DernierClignementOeil;

int centreX, centreY;
int Oeil_offsetinterne;
int vitesseOeil = OLED_DEF_VITESSE_YEUX;
int Oeil_offsetrare;
float Gyro_AngleCourant;

String strActionCourante;
String strActionPrecedente="";

// déclaration globale des objets 
SoftwareSerial bt(PIN_BLUETOOTH_RX, PIN_BLUETOOTH_TX); // définition d'un objet port Série pour le bluetooth bt(RX / TX)
SoftwareSerial mp3Serial(PIN_MP3_RX,PIN_MP3_TX); // définition d'un objet port Série pour le MP3 Player
Adafruit_SSD1306 display(OLED_RESET); // définition d'un display OLED


// définition de 2 objets moteurs DC sur les emplacement 1 et 2 du shield moteur
AF_DCMotor rmotor(4);
AF_DCMotor lmotor(2);



// ============================================================================
// **** B L U E T O O T H  ****************************************************
// ============================================================================
void Gestion_Bluetooth()
{
  // si pas de module bluetooth déclaré raccordé, on sort directement
  if(!ACTIVATE_BLUETOOTH) return;
  
  // si le bluetooth est connecté
  if (bt.available() > 0)
  {
    // on lit le contenu du buffer jusqu'au LF (qu'on doit toujours avoir, sinon ca pourrait bloquer)
    strActionCourante = bt.readStringUntil(LF);

    // on mémorise la dernière date de lecture du buffer de réception
    BT_DerniereDateReception = millis();
    Serial.println(strActionCourante);
  };
}
// ============================================================================
// **** GYRO  *****************************************************************
// ============================================================================
float Gyro_Calcule(int intervalleMesureMS)
{
  
   // on mesure le voltage reçu que l'on convertit en valeur float
   float gyroRate = (analogRead(PIN_GYROSCOPE) * GYRO_VOLTAGE) / 1023;
   // on détermine le décalage par rapport au voltage neutre de position immobile
   gyroRate = gyroRate - GYRO_ZEROVOLTAGE;
   // on divise le voltage par la sensibilité annoncée du gyro
   gyroRate = gyroRate / GYRO_SENSITIVITY;
   // si notre seuil mini de sensibilité est atteint, on tient compte de la mesure pour changer l'angle
   //if ((gyroRate>= GYRO_ROTATIONTHRESHOLD) || (gyroRate <= -GYRO_ROTATIONTHRESHOLD))
   if(true)
   {
    // on ajoute à l'angle mémorisé, l'angle indiqué.
    gyroRate =gyroRate / intervalleMesureMS * 10; //cette ligne suppose qu'on fait une mesure toutes les 10 MS pour calculer l'angle, il faudra donc utiliser la fonction Gyro_Calcule de cette façon.
    Gyro_AngleCourant = Gyro_AngleCourant + gyroRate;
   }
   // on garde notre angle dans l'espace 0-360
  if (Gyro_AngleCourant < 0)
  {
      Gyro_AngleCourant += 360;
  }
    else if (Gyro_AngleCourant > 359)
    {
    Gyro_AngleCourant -= 360;
    }
    
  return (Gyro_AngleCourant);
}


// ============================================================================
// **** M O T E U R S  ********************************************************
// ============================================================================


void Moteurs_Gauche(int action, int motorspeed=-1)
{
  // argument optionel de redéfinition de la vitesse
  if(motorspeed>-1) lmotor.setSpeed(motorspeed);
  
  // dans tous les cas on fait l'action demandée
  lmotor.run(action);
}

// effectuer l'action demandée sur le moteur de droite
void Moteurs_Droite(int action, int motorspeed=-1)
{
  // argument optionel de redéfinition de la vitesse
  if(motorspeed>-1) rmotor.setSpeed(motorspeed);
  
  // dans tous les cas on fait l'action demandée
  rmotor.run(action);
}
void Moteurs_PivoterDegre(int degre)
{
  // si pas de module gyro déclaré raccordé, on sort directement sans pivoter
  if(!ACTIVATE_GYRO) return;

  // --- on mémorise l'angle actuel (bousole magnétique absolue)
  float startAngle = Gyro_Calcule(10);
  float curAngle = startAngle;
  int temps = 0;
  // on active la rotation
  Moteurs_Gauche(FORWARD,MOTEUR_VITESSE/2);
  Moteurs_Droite(BACKWARD,MOTEUR_VITESSE/2);
  // on attend que l'angle désiré soit atteint
  do {
    delay(10);
    temps+=10; // on compte le temps qu'on a passé (pour sortir avec un time-out de GYRO_TIMEOUT_MS millisecondes, si ca dure trop)
    curAngle = Gyro_Calcule(10);
  } while (((curAngle-startAngle)<degre) && (temps<GYRO_TIMEOUT_MS));
  // on arrête les moteurs, quand c'est fait
  Moteurs_Gauche(RELEASE);
  Moteurs_Droite(RELEASE);
}


void Gestion_Moteurs()
{

  // si pas de shield moteur déclaré raccordé, on sort directement
  if(!ACTIVATE_MOTORS) return;
  
  if (strActionCourante == "AVANCER")
  {
  
    // avancer signifie activer les 2 moteurs à la même vitesse
    Moteurs_Gauche(FORWARD,MOTEUR_VITESSE);
    Moteurs_Droite(FORWARD,MOTEUR_VITESSE);
    
    delay(1000);
    Moteurs_Gauche(RELEASE);
    Moteurs_Droite(RELEASE);
  }
  else if (strActionCourante == "RECULER")
  {
    // reculer signifie activer les 2 moteurs à la même vitesse, en arrière
    Moteurs_Gauche(BACKWARD,MOTEUR_VITESSE);
    Moteurs_Droite(BACKWARD,MOTEUR_VITESSE);
    
    delay(1000);
    Moteurs_Gauche(RELEASE);
    Moteurs_Droite(RELEASE);
  }
  else if (strActionCourante == "GAUCHE")
  {
    
    // pour aller en AVANT GAUCHE, la roue gauche, va 3 x moins vite
    Moteurs_Gauche(FORWARD,MOTEUR_VITESSE/2);
    Moteurs_Droite(FORWARD,MOTEUR_VITESSE);
    delay(1000);
    Moteurs_Gauche(RELEASE);
    Moteurs_Droite(RELEASE);
  }
  else if (strActionCourante == "DROITE")
  {
    
    // pour aller en AVANT DROIT, la roue droite, va 3 x moins vite
    Moteurs_Gauche(FORWARD,MOTEUR_VITESSE);
    Moteurs_Droite(FORWARD,MOTEUR_VITESSE/2);
    delay(1000);
    Moteurs_Gauche(RELEASE);
    Moteurs_Droite(RELEASE);
  }
  else if (strActionCourante == "PIVOT:GAUCHE")
  {
    
    // pour aller en PIVOT GAUCHE, la roue gauche va en arrière, la roue droite en avant à vitesses égales
    // on divise la vitesse par 2 pour éviter les accoups.
    Moteurs_Gauche(BACKWARD,MOTEUR_VITESSE);
    Moteurs_Droite(FORWARD,MOTEUR_VITESSE);
    delay(1000);
    Moteurs_Gauche(RELEASE);
    Moteurs_Droite(RELEASE);
  }
  else if (strActionCourante == "PIVOT:DROIT")
  {
    
    // pour aller en PIVOT DROIT, la roue droite va en arrière, la roue gauche en avant à vitesses égales
    // on divise la vitesse par 2 pour éviter les accoups.
    Moteurs_Gauche(FORWARD,MOTEUR_VITESSE);
    Moteurs_Droite(BACKWARD,MOTEUR_VITESSE);
    delay(1000);
    Moteurs_Gauche(RELEASE);
    Moteurs_Droite(RELEASE);
  }
  else if (strActionCourante == "TOURNER:180")
  {
    
    Moteurs_PivoterDegre(180);
    
  }
  else if (strActionCourante == "ACTION1")
  {
    
  }
  else if (strActionCourante == "ACTION2")
  {
    
  }
  else if (strActionCourante == "ACTION3")
  {
    
  }
  else if (strActionCourante == "ACTION4")
  {
    
  }
  else
  {
    // SINON ON S'ARRETE, SI AUCUN ORDRE.
    Moteurs_Gauche(RELEASE);
    Moteurs_Droite(RELEASE);
  }

}




// ============================================================================
// **** O L E D   *************************************************************
// ============================================================================
void Gestion_Oled()
{
  // si pas d'écran OLED déclaré raccordé, on sort directement
  if(!ACTIVATE_OLED) return;
  
  // on affiche une mise à jour de l'action que si elle est différente de la précédente
  if(strActionCourante != strActionPrecedente)
  {
    vitesseOeil = OLED_DEF_VITESSE_YEUX;
    if(strActionCourante == "AUTONOME")
    {
      // on afficheu un oeil pour le mode autonome, pour éviter d'avoir le texte de l'action
      vitesseOeil = 1;
      int temp = random(8);
      temp = temp - temp / 2;
      // si un offset rare est défini, on le diminue progressivement pour le ramener à zéro
      Oeil_offsetrare = TendreVers(Oeil_offsetrare, 0, 1);
      // un offset rare, qui permet d'ajouter du naturel (parfois le robot regardera d'un coté, sans raison)
      if(UneChanceSur(60)) Oeil_offsetrare = random(128) - 64;
      Oled_AfficheOeil(random(temp) + Oeil_offsetrare);
    }

    else if(strActionCourante == "GAUCHE")
    {
      Oled_AfficheOeil(30);
    }
    else if(strActionCourante == "DROITE")
    {
      Oled_AfficheOeil(-30);
    }
    else if(strActionCourante == "ARRETER")
    {
      // on recentre le regard sans attendre
      Oled_AfficheOeil(0);
    }
    else if(strActionCourante == "HEIN?")
    {
      Serial.println("GESTION_OLED:HEIN?");
      for (int i = 0; i < 3; i++)
      {
        Oled_AfficherTexte("???", 7);
        delay(300);
        display.clearDisplay();
        display.display();
        delay(500);
      }
      Oled_AfficheOeil(-30);
      strActionCourante = "";
    }
    else
    {
      // sinon on affiche l'action pour le moment
      Oled_AfficherTexte(strActionCourante, 2);
    }

  }
}

// une routine pour afficher le texte demandé à l'écran OLED
void Oled_AfficherTexte(String strTexte, int taille)
{
  display.clearDisplay();
  display.setTextSize(taille);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(strTexte);
  display.display();
}

void Oled_AfficheOeilCligne()
{
  display.clearDisplay();
  display.drawRoundRect(32 + Oeil_offsetinterne, centreY - 2, 64, 4, 4, WHITE);
  display.display();
  delay(5);
}

void Oled_AfficheOeil(int Offset)
{

  if (OLED_DernierClignementOeil < (millis() - OLED_INTERVALLE_CLIGNEMENT_OEIL_MS + random(4000) - 2000))
  {
    OLED_DernierClignementOeil = millis();
    Oled_AfficheOeilCligne();
  }

  // on fait tendre l'offset interne vers l'offset demandé
  Oeil_offsetinterne = TendreVers(Oeil_offsetinterne, Offset, vitesseOeil);

  display.clearDisplay();
  //display.drawRoundRect(0, 0, display.width(), display.height(), 10, WHITE);
  display.drawCircle(centreX + Oeil_offsetinterne, centreY, 30, WHITE);
  display.fillCircle(centreX + Oeil_offsetinterne, centreY, 25, WHITE);
  display.fillCircle(centreX + Oeil_offsetinterne, centreY, 10, BLACK);
  display.fillCircle(centreX - 3 + Oeil_offsetinterne, centreY - 3, 4, WHITE);
  display.display();

}

// ============================================================================
// **** S O N A R *************************************************************
// ============================================================================
int Sonar_Distance()
{
  digitalWrite(PIN_SONAR_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_SONAR_TRIG, LOW);
  int cm = (pulseIn(PIN_SONAR_ECHO, HIGH) / 58);
  return cm;
}

bool Sonar_Obstacle()   // renvoie vrai si obstacle
{
  return (Sonar_Distance() < SONAR_DISTANCE_OBSTACLE_EN_CM);

}

// ============================================================================
// ****  L O G I Q U E   R O B O T ********************************************
// ============================================================================
void Robot_ForceAction(String strAction)
{
  if (strActionCourante == "AUTONOME")
  {
    // pour simplifier le mode "AUTONOME", on va juste simuler un déclenchement d'un ordre manuel, via l'appel de cette fonction
    // dans diverses situations
    strActionCourante = strAction;
    // sans oublier de réinitialiser le timer de réception bluetooth (pour qu'il se croie en commande manuelle)
    BT_DerniereDateReception = millis();
  }
  
}

void Gestion_AUTONOME()
{
  // si le mode est autonome uniquement
  if(strActionCourante = "AUTONOME")
  {
    // on fera une action uniquement avec une chance sur ROBOT_PROBA_ACTION_AUTONOME
    if(UneChanceSur(ROBOT_PROBA_ACTION_AUTONOME))
    {
      switch (int(random(10)))
      {
        case 1: strActionCourante = "GAUCHE"; break;
        case 2: strActionCourante = "DROITE"; break;
        case 3: strActionCourante = "AVANCER"; break;
        case 4: strActionCourante = "RECULER"; break;
        case 5: strActionCourante = "PIVOT:GAUCHE"; break;
        case 7: strActionCourante = "PIVOT:DROITE"; break;
        case 8: strActionCourante = "RANDOMSOUND"; break;
        default : strActionCourante = "ARRETER";
      }
    }
    
  }
}

void Gestion_Obstacle()
{
  // si pas de shield moteur déclaré raccordé, on sort directement
  if(!ACTIVATE_SONAR) return;
  
  // si on a un obstacle on tourne de 15 degrés (rien d'autre, on repassera ici la fois suivante, pour voir si on peut faire autre chose)
  if (Sonar_Obstacle())
  {
    // une chance sur deux qu'on fasse une tentative de tourner, l'autre cas on ne fait rien
    if (UneChanceSur(2)) 
      Moteurs_PivoterDegre(15);
    delay(500); // dans les 2 cas on attend un peu avant la suite.
  }
}

// ============================================================================
// **** G E N E R A L *********************************************************
// ============================================================================
// fonction qui fait se rapprocher valeur de sa cible, à chaque appel, selon le "pas" indiqué (linéaire).
int TendreVers(int valeur, int cible, int pas)
{
  // si la cible est atteinte, on ne change plus la valeur
  if(valeur == cible) return (valeur);
  // si la cible est plus haute, on incrémente la valeur
  if(valeur < cible) return bindInt(valeur + pas, valeur, cible);
  // sinon c'est le contraire, on baisse la valeur à retourner
  return bindInt(valeur - pas, cible, valeur);
}
int bindInt(int valeur, int mini, int maxi)
{
  if(valeur > maxi) return (maxi);
  if(valeur < mini) return (mini);
  return valeur;
}

bool UneChanceSur(int proba)
{
  // renvoie vrai selon la proba indiquée
  return (bool(millis()%proba==0));
}

// ============================================================================
// **** S E T U P  ************************************************************
// ============================================================================
void setup()
{

  Serial.begin(9600);
  // init OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display(); // show splashscreen
  centreX = display.width() / 2;
  centreY = display.height() / 2;
  
  // init bluetooth
  bt.begin(9600);
  bt.setTimeout(BT_READ_TIMEOUT_MS); // fixation du temps max d'attente de caractères en ms
  BT_DerniereDateReception = millis();
  OLED_DernierClignementOeil = millis();
  
  // init mp3 player
  mp3Serial.begin(9600);
  mp3_set_serial(mp3Serial);
  delay(1);
  mp3_set_volume(15);
  
  // init Gyroscope
  Gyro_AngleCourant = 0;
  
  // initialisation moteurs (arrêt et fixation de la vitesse par défaut)
  Moteurs_Gauche(RELEASE,MOTEUR_VITESSE);
  Moteurs_Droite(RELEASE,MOTEUR_VITESSE);
  
}

// ============================================================================
// **** L O O P  **************************************************************
// ============================================================================
void loop()
{
  if(ACTIVATE_BLUETOOTH)
  {
    if (bt.available())
    {
      // lecture bluetooth
      Gestion_Bluetooth();
    }
    else
    {
  
      delay(100);
    }
  }
  
  // si IDLE on actionne des scénarios indépendants (on ne gère pas le reset de milli à 50 jours, le robot ne tiendra jamais autant entre 2 reset, donc inutile).
  if ((BT_DerniereDateReception + (ROBOT_TEMPS_OISIF*1000)) < (millis()))
  {
    strActionCourante = "AUTONOME";
  }
  delay(20);
  
  Gestion_Oled();
  delay(20);
  
  Gestion_Moteurs();
  delay(20);
  
  // passage d'un mode AUTONOME à une action MANUELLE simulée
  Gestion_AUTONOME();
  delay(20);
  
  Serial.println("ACTION " + strActionCourante);
}

