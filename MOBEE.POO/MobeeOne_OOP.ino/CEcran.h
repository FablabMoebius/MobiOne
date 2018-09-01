#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include "FonctionsSTD.h"
#define OLED_DEF_VITESSE_YEUX 7 // rapport fluidité <=> vitesse d'animation
#define OLED_INTERVALLE_CLIGNEMENT_OEIL_MS 6000

class CEcran
{
  
  // *************************************************************************
  public: // PUBLIC **********************************************************
  // *************************************************************************
  
    // constructeur de la classe CEcran
    CEcran(int RS) : _display(RS)
    {
      //_display = new Adafruit_SSD1306(RS); // instanciation de l'écran à l'init de l'objet CEcran
      _OLED_DernierClignementOeil = millis(); // initialisation du dernier clignement à l'instant présent
      _display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
    };
    
    // fonction qui affiche un texte de la 'taille' indiquée après avoir vidé l'écran
    void AfficherTexte(String strTexte, int taille=10)
    {
      efface(); // on efface
      _display.setTextSize(taille); // on fixe la taille de la police
      _display.setTextColor(WHITE); // on choisit l'écriture "blanche"
      _display.setCursor(0, 0); // on définit la position du curseur à 0,0
      _display.println(strTexte); // on affiche le texte
      flip(); // on flippe l'écran
    };

    // en programation de jeu, on constitue d'abord en mémoire l'image, avec tous ses détails, puis on la flip(), c'est à dire qu'on fait un échange de la mémoire tampon vers la mémoire de l'écran.
    // en savoir plus sur le "page flipping" : http://zorrothebad.free.fr/java/tutorialfr/doublebuf.html
    void flip()
    {
      _display.display();  
    };

    // efface l'écran (en mémoire tampon si 'false' ou immédiat si 'true')
    void efface(bool blnWithFlip = false)
    {
      // on envoiela commande d'effacement à l'objet display
       _display.clearDisplay();
       // on fait un flip optionnel(si demandé), pour afficher le résultat sans attendre le prochain flip();
       if(blnWithFlip) flip();
    };

 
    void RegarderAGauche()
    {
      _vitesseOeil = OLED_DEF_VITESSE_YEUX;
      __AfficheOeil(30);
      
    };
    
    void RegarderADroite()
    {
      _vitesseOeil = OLED_DEF_VITESSE_YEUX;
      __AfficheOeil(-30);
      
    };
  
    void RegarderDevant()
    {
      _vitesseOeil = OLED_DEF_VITESSE_YEUX;
      __AfficheOeil(0);
    };
    
    void RegarderPerplexe()
    {
      // utile quand on ne comprend pas une question
      for (int i = 0; i < 3; i++)
      {
        AfficherTexte("???", 7);
        delay(300);
        efface(true); // affiche un écran vide
        delay(500);
      }
      __AfficheOeil(-30);
      
    };
    // *************************************************************************
    private: // PRIVATE ********************************************************
    // *************************************************************************
    
      
    // fonction qui affiche un oeil selon un offsetX (plus ou moins à gauche/droite)    
    void __AfficheOeil(int Offset)
  {
    // --- on cligne des paupières de temps en temps (légèrement aléatoirement)
    if (_OLED_DernierClignementOeil < (millis() - OLED_INTERVALLE_CLIGNEMENT_OEIL_MS + random(4000) - 2000))
    {
      _OLED_DernierClignementOeil = millis(); // réinit de la "date" du dernier clignement
      __OeilCligne(); // affichage du clin d'oeil
    };
  
    // on fait tendre l'offset interne vers l'offset demandé
    _Oeil_offsetinterne = TendreVers(_Oeil_offsetinterne, Offset, _vitesseOeil);
  
    efface();
    //_display.drawRoundRect(0, 0, _display.width(), _display.height(), 10, WHITE);
    _display.drawCircle(_centreX + _Oeil_offsetinterne, _centreY, 30, WHITE);
    _display.fillCircle(_centreX + _Oeil_offsetinterne, _centreY, 25, WHITE);
    _display.fillCircle(_centreX + _Oeil_offsetinterne, _centreY, 10, BLACK);
    _display.fillCircle(_centreX - 3 + _Oeil_offsetinterne, _centreY - 3, 4, WHITE);
    flip();
  
  };
    
    // fonction privée de clignement des yeux
    void __OeilCligne()
    {
      efface();
      _display.drawRoundRect(32 + _Oeil_offsetinterne, _centreY - 2, 64, 4, 4, WHITE);
      flip();
      delay(5);
    };
    
    Adafruit_SSD1306 _display;
    unsigned long _OLED_DernierClignementOeil;
    int _Oeil_offsetinterne; // un offset pour l'oeil
    int _centreX,_centreY;
    int _vitesseOeil = OLED_DEF_VITESSE_YEUX;
    int _Oeil_offsetrare;
};
