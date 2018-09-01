// pour n'ajouter qu'une seule fois le code à la compil
#ifndef FonctionsSTD_h
  #define FonctionsSTD_h
  #include "stdbool.h"

// ============================================================================
// **** G E N E R A L *********************************************************
// ============================================================================

// fonction qui fait se rapprocher valeur de sa cible, à chaque appel, selon le "pas" indiqué (linéaire).

int bindInt(int valeur, int mini, int maxi)
{
  if(valeur > maxi) return (maxi);
  if(valeur < mini) return (mini);
  return valeur;
};

int TendreVers(int valeur, int cible, int pas)
{
  // si la cible est atteinte, on ne change plus la valeur
  if(valeur == cible) return (valeur);
  // si la cible est plus haute, on incrémente la valeur
  if(valeur < cible) return bindInt(valeur + pas, valeur, cible);
  // sinon c'est le contraire, on baisse la valeur à retourner
  return bindInt(valeur - pas, cible, valeur);
};


bool UneChanceSur(int proba)
{
  // renvoie vrai selon la proba indiquée
  return ((millis()%proba==0));
};



#endif
