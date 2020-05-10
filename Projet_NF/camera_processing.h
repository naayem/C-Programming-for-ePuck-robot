#ifndef PROCESS_IMAGE_H
#define PROCESS_IMAGE_H

/* enum pour donner l'emplacement de la ligne noire
 * à droite, à gauche ou NULL si il n'y en a pas
 */
typedef enum posLine{
	L_NULL=0,
	L_DROITE,
	L_GAUCHE
} posLine;

//redonne la position de la ligne en Pixel sur une échelle de 0 - 600
uint16_t get_line_position(void);

//fonction qui capture et traite une image
void process_image_start(void);

/* fonction qui renvoie l'emplacement (droite/gauche) de la ligne noire
 * sur la feuille par rapport au robot si cette dernière est à proximité
 *
 * posLine - Renvoie un des trois enum qu'on trouve plus haut dans le module
 */
posLine close_line(void);

#endif
