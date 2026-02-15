#ifndef PMAN_MISC_H
#define PMAN_MISC_H

void UpdateParticles( void );
bool IsGamePaused( void );
long MaxParticleClassSize( unsigned long lSize );

void VectorScale( const float* in, float scale, float* out );
void VectorAngles( const float* forward, float* angles );
float VectorNormalize( float* v );
void VectorMA( const float* veca, float scale, const float* vecb, float* vecc );

extern float g_flOldTime;
extern Vector g_vViewAngles;
extern float g_flGravity;

extern int g_lMaxParticleClassSize;

extern struct cl_enginefuncs_s gEngfuncs;

#endif //PMAN_MISC_H