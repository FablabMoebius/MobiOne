class CSonar
{
  public:
    // constructeur CSonar
    CSonar(int pinTRIG, int pinECHO,int distObstacle)
    {
      _pinTRIG = pinTRIG;
      _pinECHO = pinECHO;
      _distObstacle = distObstacle;
    };
    
    // distance en CM de l'obstacle mesuré
    int distance()
    {
      digitalWrite(_pinTRIG, HIGH);
      delayMicroseconds(10);
      digitalWrite(_pinTRIG, LOW);
      int cm = (pulseIn(_pinECHO, HIGH) / 58);
      return cm;
    };
    
    // déclencheur de l'obstacle selon distObstacle définie
    bool obstacle()
    {
      // si la distance est inférieure à la distance d'obstacle configurée, on renvoie true
      return(distance()<_distObstacle);
    };
    
  private:
    int _pinTRIG;
    int _pinECHO;
    int _distObstacle;
};
