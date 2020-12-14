#ifndef ENEMY_STAT_DEFINE_H
#define ENEMY_STAT_DEFINE_H

/*
This class holds the base stats of enemies and how we scale them
Any scale define is multiplied by a difficulty scalar in EnemyFactory
*/

#define DIFFICULTY_PER_SECOND 0.5f
#define DIFFICULTY_PER_LEVEL 200

/*--------- ZOMBIE -------*/
//Health
#define ZOMBIE_BASE_HEALTH 50
#define ZOMBIE_SCALE_HEALTH 1.0f / 20.0f //10hp per level
//Damage
#define ZOMBIE_BASE_DAMAGE 20
#define ZOMBIE_SCALE_DAMAGE 1.0f / 60.0f //3.33 damage per level
//Attack Speed
#define ZOMBIE_BASE_ATTACK_SPEED 1.0f
//#define ZOMBIE_SCALE_ATTACK_SPEED 0 <---- this is not implemented
#define ZOMBIE_BASE_ATTACK_INTERVAL 0.5f
//#define ZOMBIE_SCALE_ATTACK_INTERVAL 0 <--- this is not implemented
//Movement Speed
#define ZOMBIE_BASE_SPEED 45.0f
//#define ZOMBIE_SCALE_SPEED 0 <--- this is not implemented
//Slowing Attack
#define ZOMBIE_SLOW_ATTACK 0.5f
//#define ZOMBIE_SCALE_SLOW_ATTACK 0 <--- this is not implemented
//Attack Range
#define ZOMBIE_BASE_ATTACK_RANGE 1.5f
//#define ZOMBIE_SCALE_ATTACK_RANGE 0 <--- this is not implemented
//Detection Range
#define ZOMBIE_BASE_DETECTION 150.0f
//#define ZOMBIE_SCALE_DETECTION 0 <--- this is not implemented

/*------- SPIDER ------*/
//Health
#define SPIDER_BASE_HEALTH 25
#define SPIDER_SCALE_HEALTH 1.0f / 60.0f //3.33hp per level
//Damage
#define SPIDER_BASE_DAMAGE 10
#define SPIDER_SCALE_DAMAGE 1.0f / 120.0f //1.66 damage per level
//Attack Speed
#define SPIDER_BASE_ATTACK_SPEED 0.05f
//#define SPIDER_SCALE_ATTACK_SPEED 0 <---- this is not implemented
#define SPIDER_BASE_ATTACK_INTERVAL 0.7f
//#define SPIDER_SCALE_ATTACK_INTERVAL 0 <--- this is not implemented
//Movement Speed
#define SPIDER_BASE_SPEED 90.0f
//#define SPIDER_SCALE_SPEED 0 <--- this is not implemented
//Slowing Attack
#define SPIDER_SLOW_ATTACK 0
//#define SPIDER_SCALE_SLOW_ATTACK 0 <--- this is not implemented
//Attack Range
#define SPIDER_BASE_ATTACK_RANGE 1.5f
//#define SPIDER_SCALE_ATTACK_RANGE 0 <--- this is not implemented
//Detection Range
#define SPIDER_BASE_DETECTION 500.0f
//#define SPIDER_SCALE_DETECTION 0 <--- this is not implemented

/*------- DEMON ------*/
//Health
#define DEMON_BASE_HEALTH 90
#define DEMON_SCALE_HEALTH 1.0f / 19.0f //10.5hp per level
//Damage
#define DEMON_BASE_DAMAGE 50
#define DEMON_SCALE_DAMAGE 1.0f / 50.0f //4 damage per level
//Attack Speed
#define DEMON_BASE_ATTACK_SPEED 1.5f
//#define DEMON_SCALE_ATTACK_SPEED 0 <---- this is not implemented
#define DEMON_BASE_ATTACK_INTERVAL 0.5f
//#define DEMON_SCALE_ATTACK_INTERVAL 0 <--- this is not implemented
//Movement Speed
#define DEMON_BASE_SPEED 30.0f
//#define DEMON_SCALE_SPEED 0 <--- this is not implemented
//Attack Range
#define DEMON_BASE_ATTACK_RANGE 100.0f
//#define DEMON_SCALE_ATTACK_RANGE 0 <--- this is not implemented
//Detection Range
#define DEMON_BASE_DETECTION 500.0f
//#define SPIDER_SCALE_DETECTION 0 <--- this is not implemented

#endif