#pragma once

// useful constants that are used throughout the program for the number generators
const float VECTOR_LENGTH_CUTOFF_POINT = 1e-6f;
static unsigned long SEED = 2791677072;   // set random seed to control all the randomness


const int MAX_LEAVES = 6;
const int MIN_LEAVES = 2;
const int MAX_BRANCHES = 20;
const int MIN_BRANCHES = 5;
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
    500.0f,
    1000.f,
    0.85f,
    0.85f,
    0.35f,
};

// specifically for the panicle branches can be adjusted later
inline constexpr SPINE_CONFIG GRASS_BRANCH{
    80.0f,
    150.0f,
    0.0f,
    0.0f,
    0.0f,
};