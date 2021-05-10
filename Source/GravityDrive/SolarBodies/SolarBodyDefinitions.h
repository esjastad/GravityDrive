// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"

/**
 * Definitions and ENUMS used in the procedural generation of galaxies and solar bodies
 */
#define EVENTHORIZON 255000000000
#define SOLARSYSTEMAVERAGE 600000000000000
#define SUPERBLACKHOLEMAX 20000
#define SUPERBLACKHOLEMIN 60
#define STARSCALEMAX 4100
#define STARSCALEMIN 0.01
#define GASGIANTMAX 2
#define GASGIANTMIN 1
#define PLANETMAX 16
#define PLANETMIN 8

#define STARRADIUS 17186210817
#define GASGIANTRADIUS 7057820000
#define TERRESTRIALRADIUS 629660000
#define MOONRADIUS 157420000

enum SolarBodyType {
	SBSun,
	SBGasGiant,
	SBTerrestrial,
	SBMoon
};
