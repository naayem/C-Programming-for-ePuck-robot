#ifndef PROCESS_IMAGE_H
#define PROCESS_IMAGE_H

/*enum pour donner l'emplacement de la ligne noire
 * � droite, � gauche ou NULL si il n'y en a pas
 */
typedef enum posLine{
	L_NULL=0,
	L_DROITE,
	L_GAUCHE
} posLine;

uint16_t get_line_position(void);
void process_image_start(void);

/*fonction qui renvoie l'emplacement (droite/gauche) de la ligne noire
 * sur la feuille par rapport au robot si cette derni�re est � proximit�
 *
 * posLine - Renvoie un des trois enum qu'on trouve plus haut dans le module
 */
posLine close_line(void);

#endif
