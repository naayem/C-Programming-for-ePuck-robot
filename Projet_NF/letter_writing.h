#ifndef LETTER_WRITING_H
#define LETTER_WRITING_H

/*fonctions qui permettent au robot de faire le d�placement suivant la forme
 * g�om�trique correspondant � chaque lettre �crite dans la fonction.
 * Le but premier est d'avoir des formes ressemblantes!
 */
void lettre_M (void);
void lettre_O (void);
void lettre_N (void);
void lettre_D (void);
void lettre_A (void);

/*fonction qui s'occupe de d�caler le robot
 * pour ne pas �crire les lettres les unes sur les autres
 */
void decalage_interlettre(void);

#endif
