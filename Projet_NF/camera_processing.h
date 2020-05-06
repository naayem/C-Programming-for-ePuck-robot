#ifndef PROCESS_IMAGE_H
#define PROCESS_IMAGE_H

/*enum pour donner l'emplacement de la ligne noire
 * à droite, à gauche ou NULL si il n'y en a pas
 */
typedef enum posLine{
	L_NULL=0,
	L_DROITE,
	L_GAUCHE
} posLine;

uint16_t get_line_position(void);
void process_image_start(void);

/*fonction qui renvoie l'emplacement (droite/gauche) de la ligne noire
 * sur la feuille par rapport au robot si cette dernière est à proximité
 */
posLine close_line(void);

#endif
