#pragma once

// useful constants that are used throughout the program for the number generators
const float VECTOR_LENGTH_CUTOFF_POINT = 1e-6f;
static unsigned long SEED = 2791677072;   // set random seed to control all the randomness

struct SPINE_CONFIG {
    float minLength = 500.0f;
    float maxLength = 1000.0f;
    float maxLean = 0.85f;
    float minLean = 0.15f;
    float maxCurve = 0.85f;
    float minCurve = 0.15f;
    float maxDroop = 0.45f;
    float minDroop = 0.10f;
};
static const SPINE_CONFIG GRASS_SPINE;

struct STEM_CONFIG {
    float threePointChance = 0.51f;
    int bezierResolution = 100;
    float baseRadius = 10.0f;
    float tipRadius = 1.0f;
    int radialSegments = 20;
};
static const STEM_CONFIG STEM_PARAMS;

struct LEAF_CONFIG {
    float minPlacement = 0.15f;
    float maxPlacement = 0.45f;
    float baseOffset = 0.3f;
    float minLengthFactor = 0.10f;
    float maxLengthFactor = 0.75f;
    float bezierOut = 0.18f;
    float bezierUp = 0.30f;
    float bezierLean = 0.75f;
    float bezierDroop = 0.55f;
    int bezierResolution = 50;
    float widthMinFactor = 1.0f;
    float widthMaxFactor = 3.5f;
};
static const LEAF_CONFIG LEAF_PARAMS;

struct BRANCH_GEN_CONFIG {
    float minPlacement = 0.25f;
    float maxPlacement = 0.90f;
    float outwardFactor = 0.3f;
    float upwardFactor = 1.0f;
    float minLength = 80.0f;
    float maxLength = 150.0f;
    float minRadius = 1.0f;
    float maxRadius = 3.0f;
    int radialResolution = 3;
    int lengthResolution = 1;
};
static const BRANCH_GEN_CONFIG BRANCH_GEN_PARAMS;

struct SEED_CONFIG {
    int resolution = 8;
    float scaleXY = 0.5f;
    float minScaleZ = 2.0f;
    float maxScaleZ = 3.0f;
    float minRadius = 2.0f;
    float maxRadius = 4.0f;
};
static const SEED_CONFIG SEED_PARAMS;

struct FIELD_CONFIG {
    int numSinglePlants = 250;
    int numPlants = 1000;
    float range = 2000.0f;
};
static const FIELD_CONFIG FIELD_PARAMS;

const int MAX_LEAVES = 6;
const int MIN_LEAVES = 2;
const int MAX_BRANCHES = 20;
const int MIN_BRANCHES = 5;
