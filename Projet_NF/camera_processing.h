#ifndef PROCESS_IMAGE_H
#define PROCESS_IMAGE_H


typedef enum posLine{
	L_NULL=0,
	L_MILIEU,
	L_DROITE,
	L_GAUCHE
} posLine;

float get_distance_cm(void);
uint16_t get_line_position(void);
void process_image_start(void);
posLine close_line(void);



#endif /* PROCESS_IMAGE_H */
