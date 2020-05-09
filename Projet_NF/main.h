#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "camera/dcmi_camera.h"
#include "msgbus/messagebus.h"
#include "parameter/parameter.h"
#include <chprintf.h>

extern parameter_namespace_t parameter_root;

void SendUint8ToComputer(uint8_t* data, uint16_t size);

/** Robot wide IPC bus. */
extern messagebus_t bus;

extern parameter_namespace_t parameter_root;

//Enumeration des états de la machine d'état qui regit les modes de jeux dans lequelle peut etre le robot
typedef enum {
MENU_PRINCIPAL=0,
PONG_INIT,
PONG,
ALPHABET,
BILLARD_INIT,
BILLARD,
ENDGAME
} etats;

#ifdef __cplusplus
}
#endif

#endif
