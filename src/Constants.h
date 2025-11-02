#pragma once

// useful constants that are used throughout the program
const float VECTOR_LENGTH_CUTOFF_POINT = 1e-6f;
static unsigned long SEED = 4267694201999;   // set random seed to control all the randomness
// struct to vary parameters of the grass spine (maybe the branches themselves as well)
const struct SPINE_CONFIG {
    float minLength;
    float maxLength; 
    float maxLean;                   
    float maxCurve;                    
    float maxDroop;                    
};

// specifically for the spine can be adjusted later
inline constexpr SPINE_CONFIG GRASS_SPINE{
    200.0f,
    500.f,
    0.85f,
    0.85f,
    0.35f,
};