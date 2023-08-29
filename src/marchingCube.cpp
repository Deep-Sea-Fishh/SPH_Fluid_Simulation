#include "marchingCube.h"
#include <iostream>
const unsigned int edgeTable[256] = {
    0x0, 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
    0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
    0x190, 0x99, 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
    0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
    0x230, 0x339, 0x33, 0x13a, 0x636, 0x73f, 0x435, 0x53c,
    0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
    0x3a0, 0x2a9, 0x1a3, 0xaa, 0x7a6, 0x6af, 0x5a5, 0x4ac,
    0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
    0x460, 0x569, 0x663, 0x76a, 0x66, 0x16f, 0x265, 0x36c,
    0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
    0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff, 0x3f5, 0x2fc,
    0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
    0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55, 0x15c,
    0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
    0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc,
    0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
    0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
    0xcc, 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
    0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
    0x15c, 0x55, 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
    0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
    0x2fc, 0x3f5, 0xff, 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
    0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
    0x36c, 0x265, 0x16f, 0x66, 0x76a, 0x663, 0x569, 0x460,
    0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
    0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa, 0x1a3, 0x2a9, 0x3a0,
    0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
    0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33, 0x339, 0x230,
    0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
    0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99, 0x190,
    0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
    0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0};
#define X 255
const int triTable[256][16] = {
    {X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X},
    {0, 8, 3, X, X, X, X, X, X, X, X, X, X, X, X, X},
    {0, 1, 9, X, X, X, X, X, X, X, X, X, X, X, X, X},
    {1, 8, 3, 9, 8, 1, X, X, X, X, X, X, X, X, X, X},
    {1, 2, 10, X, X, X, X, X, X, X, X, X, X, X, X, X},
    {0, 8, 3, 1, 2, 10, X, X, X, X, X, X, X, X, X, X},
    {9, 2, 10, 0, 2, 9, X, X, X, X, X, X, X, X, X, X},
    {2, 8, 3, 2, 10, 8, 10, 9, 8, X, X, X, X, X, X, X},
    {3, 11, 2, X, X, X, X, X, X, X, X, X, X, X, X, X},
    {0, 11, 2, 8, 11, 0, X, X, X, X, X, X, X, X, X, X},
    {1, 9, 0, 2, 3, 11, X, X, X, X, X, X, X, X, X, X},
    {1, 11, 2, 1, 9, 11, 9, 8, 11, X, X, X, X, X, X, X},
    {3, 10, 1, 11, 10, 3, X, X, X, X, X, X, X, X, X, X},
    {0, 10, 1, 0, 8, 10, 8, 11, 10, X, X, X, X, X, X, X},
    {3, 9, 0, 3, 11, 9, 11, 10, 9, X, X, X, X, X, X, X},
    {9, 8, 10, 10, 8, 11, X, X, X, X, X, X, X, X, X, X},
    {4, 7, 8, X, X, X, X, X, X, X, X, X, X, X, X, X},
    {4, 3, 0, 7, 3, 4, X, X, X, X, X, X, X, X, X, X},
    {0, 1, 9, 8, 4, 7, X, X, X, X, X, X, X, X, X, X},
    {4, 1, 9, 4, 7, 1, 7, 3, 1, X, X, X, X, X, X, X},
    {1, 2, 10, 8, 4, 7, X, X, X, X, X, X, X, X, X, X},
    {3, 4, 7, 3, 0, 4, 1, 2, 10, X, X, X, X, X, X, X},
    {9, 2, 10, 9, 0, 2, 8, 4, 7, X, X, X, X, X, X, X},
    {2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, X, X, X, X},
    {8, 4, 7, 3, 11, 2, X, X, X, X, X, X, X, X, X, X},
    {11, 4, 7, 11, 2, 4, 2, 0, 4, X, X, X, X, X, X, X},
    {9, 0, 1, 8, 4, 7, 2, 3, 11, X, X, X, X, X, X, X},
    {4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, X, X, X, X},
    {3, 10, 1, 3, 11, 10, 7, 8, 4, X, X, X, X, X, X, X},
    {1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, X, X, X, X},
    {4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, X, X, X, X},
    {4, 7, 11, 4, 11, 9, 9, 11, 10, X, X, X, X, X, X, X},
    {9, 5, 4, X, X, X, X, X, X, X, X, X, X, X, X, X},
    {9, 5, 4, 0, 8, 3, X, X, X, X, X, X, X, X, X, X},
    {0, 5, 4, 1, 5, 0, X, X, X, X, X, X, X, X, X, X},
    {8, 5, 4, 8, 3, 5, 3, 1, 5, X, X, X, X, X, X, X},
    {1, 2, 10, 9, 5, 4, X, X, X, X, X, X, X, X, X, X},
    {3, 0, 8, 1, 2, 10, 4, 9, 5, X, X, X, X, X, X, X},
    {5, 2, 10, 5, 4, 2, 4, 0, 2, X, X, X, X, X, X, X},
    {2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, X, X, X, X},
    {9, 5, 4, 2, 3, 11, X, X, X, X, X, X, X, X, X, X},
    {0, 11, 2, 0, 8, 11, 4, 9, 5, X, X, X, X, X, X, X},
    {0, 5, 4, 0, 1, 5, 2, 3, 11, X, X, X, X, X, X, X},
    {2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, X, X, X, X},
    {10, 3, 11, 10, 1, 3, 9, 5, 4, X, X, X, X, X, X, X},
    {4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, X, X, X, X},
    {5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, X, X, X, X},
    {5, 4, 8, 5, 8, 10, 10, 8, 11, X, X, X, X, X, X, X},
    {9, 7, 8, 5, 7, 9, X, X, X, X, X, X, X, X, X, X},
    {9, 3, 0, 9, 5, 3, 5, 7, 3, X, X, X, X, X, X, X},
    {0, 7, 8, 0, 1, 7, 1, 5, 7, X, X, X, X, X, X, X},
    {1, 5, 3, 3, 5, 7, X, X, X, X, X, X, X, X, X, X},
    {9, 7, 8, 9, 5, 7, 10, 1, 2, X, X, X, X, X, X, X},
    {10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, X, X, X, X},
    {8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, X, X, X, X},
    {2, 10, 5, 2, 5, 3, 3, 5, 7, X, X, X, X, X, X, X},
    {7, 9, 5, 7, 8, 9, 3, 11, 2, X, X, X, X, X, X, X},
    {9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, X, X, X, X},
    {2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, X, X, X, X},
    {11, 2, 1, 11, 1, 7, 7, 1, 5, X, X, X, X, X, X, X},
    {9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, X, X, X, X},
    {5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, X},
    {11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, X},
    {11, 10, 5, 7, 11, 5, X, X, X, X, X, X, X, X, X, X},
    {10, 6, 5, X, X, X, X, X, X, X, X, X, X, X, X, X},
    {0, 8, 3, 5, 10, 6, X, X, X, X, X, X, X, X, X, X},
    {9, 0, 1, 5, 10, 6, X, X, X, X, X, X, X, X, X, X},
    {1, 8, 3, 1, 9, 8, 5, 10, 6, X, X, X, X, X, X, X},
    {1, 6, 5, 2, 6, 1, X, X, X, X, X, X, X, X, X, X},
    {1, 6, 5, 1, 2, 6, 3, 0, 8, X, X, X, X, X, X, X},
    {9, 6, 5, 9, 0, 6, 0, 2, 6, X, X, X, X, X, X, X},
    {5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, X, X, X, X},
    {2, 3, 11, 10, 6, 5, X, X, X, X, X, X, X, X, X, X},
    {11, 0, 8, 11, 2, 0, 10, 6, 5, X, X, X, X, X, X, X},
    {0, 1, 9, 2, 3, 11, 5, 10, 6, X, X, X, X, X, X, X},
    {5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, X, X, X, X},
    {6, 3, 11, 6, 5, 3, 5, 1, 3, X, X, X, X, X, X, X},
    {0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, X, X, X, X},
    {3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, X, X, X, X},
    {6, 5, 9, 6, 9, 11, 11, 9, 8, X, X, X, X, X, X, X},
    {5, 10, 6, 4, 7, 8, X, X, X, X, X, X, X, X, X, X},
    {4, 3, 0, 4, 7, 3, 6, 5, 10, X, X, X, X, X, X, X},
    {1, 9, 0, 5, 10, 6, 8, 4, 7, X, X, X, X, X, X, X},
    {10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, X, X, X, X},
    {6, 1, 2, 6, 5, 1, 4, 7, 8, X, X, X, X, X, X, X},
    {1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, X, X, X, X},
    {8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, X, X, X, X},
    {7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, X},
    {3, 11, 2, 7, 8, 4, 10, 6, 5, X, X, X, X, X, X, X},
    {5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, X, X, X, X},
    {0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, X, X, X, X},
    {9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, X},
    {8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, X, X, X, X},
    {5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, X},
    {0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, X},
    {6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, X, X, X, X},
    {10, 4, 9, 6, 4, 10, X, X, X, X, X, X, X, X, X, X},
    {4, 10, 6, 4, 9, 10, 0, 8, 3, X, X, X, X, X, X, X},
    {10, 0, 1, 10, 6, 0, 6, 4, 0, X, X, X, X, X, X, X},
    {8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, X, X, X, X},
    {1, 4, 9, 1, 2, 4, 2, 6, 4, X, X, X, X, X, X, X},
    {3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, X, X, X, X},
    {0, 2, 4, 4, 2, 6, X, X, X, X, X, X, X, X, X, X},
    {8, 3, 2, 8, 2, 4, 4, 2, 6, X, X, X, X, X, X, X},
    {10, 4, 9, 10, 6, 4, 11, 2, 3, X, X, X, X, X, X, X},
    {0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, X, X, X, X},
    {3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, X, X, X, X},
    {6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, X},
    {9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, X, X, X, X},
    {8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, X},
    {3, 11, 6, 3, 6, 0, 0, 6, 4, X, X, X, X, X, X, X},
    {6, 4, 8, 11, 6, 8, X, X, X, X, X, X, X, X, X, X},
    {7, 10, 6, 7, 8, 10, 8, 9, 10, X, X, X, X, X, X, X},
    {0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, X, X, X, X},
    {10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, X, X, X, X},
    {10, 6, 7, 10, 7, 1, 1, 7, 3, X, X, X, X, X, X, X},
    {1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, X, X, X, X},
    {2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, X},
    {7, 8, 0, 7, 0, 6, 6, 0, 2, X, X, X, X, X, X, X},
    {7, 3, 2, 6, 7, 2, X, X, X, X, X, X, X, X, X, X},
    {2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, X, X, X, X},
    {2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, X},
    {1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, X},
    {11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, X, X, X, X},
    {8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, X},
    {0, 9, 1, 11, 6, 7, X, X, X, X, X, X, X, X, X, X},
    {7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, X, X, X, X},
    {7, 11, 6, X, X, X, X, X, X, X, X, X, X, X, X, X},
    {7, 6, 11, X, X, X, X, X, X, X, X, X, X, X, X, X},
    {3, 0, 8, 11, 7, 6, X, X, X, X, X, X, X, X, X, X},
    {0, 1, 9, 11, 7, 6, X, X, X, X, X, X, X, X, X, X},
    {8, 1, 9, 8, 3, 1, 11, 7, 6, X, X, X, X, X, X, X},
    {10, 1, 2, 6, 11, 7, X, X, X, X, X, X, X, X, X, X},
    {1, 2, 10, 3, 0, 8, 6, 11, 7, X, X, X, X, X, X, X},
    {2, 9, 0, 2, 10, 9, 6, 11, 7, X, X, X, X, X, X, X},
    {6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, X, X, X, X},
    {7, 2, 3, 6, 2, 7, X, X, X, X, X, X, X, X, X, X},
    {7, 0, 8, 7, 6, 0, 6, 2, 0, X, X, X, X, X, X, X},
    {2, 7, 6, 2, 3, 7, 0, 1, 9, X, X, X, X, X, X, X},
    {1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, X, X, X, X},
    {10, 7, 6, 10, 1, 7, 1, 3, 7, X, X, X, X, X, X, X},
    {10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, X, X, X, X},
    {0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, X, X, X, X},
    {7, 6, 10, 7, 10, 8, 8, 10, 9, X, X, X, X, X, X, X},
    {6, 8, 4, 11, 8, 6, X, X, X, X, X, X, X, X, X, X},
    {3, 6, 11, 3, 0, 6, 0, 4, 6, X, X, X, X, X, X, X},
    {8, 6, 11, 8, 4, 6, 9, 0, 1, X, X, X, X, X, X, X},
    {9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, X, X, X, X},
    {6, 8, 4, 6, 11, 8, 2, 10, 1, X, X, X, X, X, X, X},
    {1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, X, X, X, X},
    {4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, X, X, X, X},
    {10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, X},
    {8, 2, 3, 8, 4, 2, 4, 6, 2, X, X, X, X, X, X, X},
    {0, 4, 2, 4, 6, 2, X, X, X, X, X, X, X, X, X, X},
    {1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, X, X, X, X},
    {1, 9, 4, 1, 4, 2, 2, 4, 6, X, X, X, X, X, X, X},
    {8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, X, X, X, X},
    {10, 1, 0, 10, 0, 6, 6, 0, 4, X, X, X, X, X, X, X},
    {4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, X},
    {10, 9, 4, 6, 10, 4, X, X, X, X, X, X, X, X, X, X},
    {4, 9, 5, 7, 6, 11, X, X, X, X, X, X, X, X, X, X},
    {0, 8, 3, 4, 9, 5, 11, 7, 6, X, X, X, X, X, X, X},
    {5, 0, 1, 5, 4, 0, 7, 6, 11, X, X, X, X, X, X, X},
    {11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, X, X, X, X},
    {9, 5, 4, 10, 1, 2, 7, 6, 11, X, X, X, X, X, X, X},
    {6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, X, X, X, X},
    {7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, X, X, X, X},
    {3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, X},
    {7, 2, 3, 7, 6, 2, 5, 4, 9, X, X, X, X, X, X, X},
    {9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, X, X, X, X},
    {3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, X, X, X, X},
    {6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, X},
    {9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, X, X, X, X},
    {1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, X},
    {4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, X},
    {7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, X, X, X, X},
    {6, 9, 5, 6, 11, 9, 11, 8, 9, X, X, X, X, X, X, X},
    {3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, X, X, X, X},
    {0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, X, X, X, X},
    {6, 11, 3, 6, 3, 5, 5, 3, 1, X, X, X, X, X, X, X},
    {1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, X, X, X, X},
    {0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, X},
    {11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, X},
    {6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, X, X, X, X},
    {5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, X, X, X, X},
    {9, 5, 6, 9, 6, 0, 0, 6, 2, X, X, X, X, X, X, X},
    {1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, X},
    {1, 5, 6, 2, 1, 6, X, X, X, X, X, X, X, X, X, X},
    {1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, X},
    {10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, X, X, X, X},
    {0, 3, 8, 5, 6, 10, X, X, X, X, X, X, X, X, X, X},
    {10, 5, 6, X, X, X, X, X, X, X, X, X, X, X, X, X},
    {11, 5, 10, 7, 5, 11, X, X, X, X, X, X, X, X, X, X},
    {11, 5, 10, 11, 7, 5, 8, 3, 0, X, X, X, X, X, X, X},
    {5, 11, 7, 5, 10, 11, 1, 9, 0, X, X, X, X, X, X, X},
    {10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, X, X, X, X},
    {11, 1, 2, 11, 7, 1, 7, 5, 1, X, X, X, X, X, X, X},
    {0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, X, X, X, X},
    {9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, X, X, X, X},
    {7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, X},
    {2, 5, 10, 2, 3, 5, 3, 7, 5, X, X, X, X, X, X, X},
    {8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, X, X, X, X},
    {9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, X, X, X, X},
    {9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, X},
    {1, 3, 5, 3, 7, 5, X, X, X, X, X, X, X, X, X, X},
    {0, 8, 7, 0, 7, 1, 1, 7, 5, X, X, X, X, X, X, X},
    {9, 0, 3, 9, 3, 5, 5, 3, 7, X, X, X, X, X, X, X},
    {9, 8, 7, 5, 9, 7, X, X, X, X, X, X, X, X, X, X},
    {5, 8, 4, 5, 10, 8, 10, 11, 8, X, X, X, X, X, X, X},
    {5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, X, X, X, X},
    {0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, X, X, X, X},
    {10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, X},
    {2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, X, X, X, X},
    {0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, X},
    {0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, X},
    {9, 4, 5, 2, 11, 3, X, X, X, X, X, X, X, X, X, X},
    {2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, X, X, X, X},
    {5, 10, 2, 5, 2, 4, 4, 2, 0, X, X, X, X, X, X, X},
    {3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, X},
    {5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, X, X, X, X},
    {8, 4, 5, 8, 5, 3, 3, 5, 1, X, X, X, X, X, X, X},
    {0, 4, 5, 1, 0, 5, X, X, X, X, X, X, X, X, X, X},
    {8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, X, X, X, X},
    {9, 4, 5, X, X, X, X, X, X, X, X, X, X, X, X, X},
    {4, 11, 7, 4, 9, 11, 9, 10, 11, X, X, X, X, X, X, X},
    {0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, X, X, X, X},
    {1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, X, X, X, X},
    {3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, X},
    {4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, X, X, X, X},
    {9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, X},
    {11, 7, 4, 11, 4, 2, 2, 4, 0, X, X, X, X, X, X, X},
    {11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, X, X, X, X},
    {2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, X, X, X, X},
    {9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, X},
    {3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, X},
    {1, 10, 2, 8, 7, 4, X, X, X, X, X, X, X, X, X, X},
    {4, 9, 1, 4, 1, 7, 7, 1, 3, X, X, X, X, X, X, X},
    {4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, X, X, X, X},
    {4, 0, 3, 7, 4, 3, X, X, X, X, X, X, X, X, X, X},
    {4, 8, 7, X, X, X, X, X, X, X, X, X, X, X, X, X},
    {9, 10, 8, 10, 11, 8, X, X, X, X, X, X, X, X, X, X},
    {3, 0, 9, 3, 9, 11, 11, 9, 10, X, X, X, X, X, X, X},
    {0, 1, 10, 0, 10, 8, 8, 10, 11, X, X, X, X, X, X, X},
    {3, 1, 10, 11, 3, 10, X, X, X, X, X, X, X, X, X, X},
    {1, 2, 11, 1, 11, 9, 9, 11, 8, X, X, X, X, X, X, X},
    {3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, X, X, X, X},
    {0, 2, 11, 8, 0, 11, X, X, X, X, X, X, X, X, X, X},
    {3, 2, 11, X, X, X, X, X, X, X, X, X, X, X, X, X},
    {2, 3, 8, 2, 8, 10, 10, 8, 9, X, X, X, X, X, X, X},
    {9, 10, 2, 0, 9, 2, X, X, X, X, X, X, X, X, X, X},
    {2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, X, X, X, X},
    {1, 10, 2, X, X, X, X, X, X, X, X, X, X, X, X, X},
    {1, 3, 8, 9, 1, 8, X, X, X, X, X, X, X, X, X, X},
    {0, 9, 1, X, X, X, X, X, X, X, X, X, X, X, X, X},
    {0, 3, 8, X, X, X, X, X, X, X, X, X, X, X, X, X},
    {X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X}};
rxMCMesh::rxMCMesh()
{
    m_Grid.fMin = glm::vec3(0.0);
    m_Grid.fWidth = glm::vec3(0.0);
    m_Grid.iNum[0] = 0;
    m_Grid.iNum[1] = 0;
    m_Grid.iNum[2] = 0;

    m_nTriangles = 0;
    m_nNormals = 0;
    m_nVertices = 0;

    m_ptScalarField = nullptr;
    m_tIsoLevel = 0;
    m_bValidSurface = false;
}

rxMCMesh::~rxMCMesh()
{
    DeleteSurface();
}

void rxMCMesh::DeleteSurface()
{
    m_Grid.fWidth[0] = 0;
    m_Grid.fWidth[1] = 0;
    m_Grid.fWidth[2] = 0;
    m_Grid.iNum[0] = 0;
    m_Grid.iNum[1] = 0;
    m_Grid.iNum[2] = 0;

    m_nTriangles = 0;
    m_nNormals = 0;
    m_nVertices = 0;

    m_ptScalarField = NULL;
    m_tIsoLevel = 0;
    m_bValidSurface = false;
}

//*从样本成三角形网格
//* @param [in]字段样本量
//* @param [in] min_p网格的最小坐标
//* @param [in] h网格的宽度
//* @param [in] n [3]网格编号（x，y，z）
//* @param [in]阈值阈值（隐含函数值网格化此值）
//* @param [in]方法网格生成方法（“mc”，“rmt”，“bloomenthal”）
//* @param [out] vrts顶点坐标
//* @param [out] nrms顶点正常
//* @param [out] tris mesh
//* @ retval true Mesh代成功
//* @retval false网格生成失败

bool rxMCMesh::CreateMeshV(float *field, glm::vec3 min_p, double h, int *n, float threshold, vector<glm::vec3> &vrts, vector<glm::vec3> &nrms, vector<rxFace> &face)
{
    if (field == nullptr)
        return false;

    RxScalarField sf;
    for (int i = 0; i < 3; ++i)
    {
        sf.iNum[i] = n[i];
        sf.fWidth[i] = h;
        sf.fMin[i] = min_p[i];
    }

    vector<int> tris;
    GenerateSurfaceV(sf, field, threshold, vrts, nrms, tris);

    if (IsSurfaceValid())
    {
        int nv = (int)GetNumVertices();
        int nm = (int)GetNumTriangles();
        int nn = (int)GetNumNormals();

        for (int i = 0; i < nn; ++i)
        {
            nrms[i] *= -1.0;
        }

        face.resize(nm);
        for (int i = 0; i < nm; ++i)
        {
            face[i].vert_idx.resize(3);
            for (int j = 0; j < 3; ++j)
            {
                face[i][j] = tris[3 * i + (2 - j)];
            }
        }

        return true;
    }

    return false;
}

void rxMCMesh::GenerateSurfaceV(const RxScalarField sf, float *field, float threshold,
                                vector<glm::vec3> &vrts, vector<glm::vec3> &nrms, vector<int> &tris)
{
    // 等值面生成
    if (m_bValidSurface)
    {
        DeleteSurface();
    }

    m_tIsoLevel = threshold;
    m_Grid.iNum[0] = sf.iNum[0];
    m_Grid.iNum[1] = sf.iNum[1];
    m_Grid.iNum[2] = sf.iNum[2];
    m_Grid.fWidth = sf.fWidth;
    m_Grid.fMin = sf.fMin;
    m_ptScalarField = field;

    unsigned int slice0 = (m_Grid.iNum[0] + 1);
    unsigned int slice1 = slice0 * (m_Grid.iNum[1] + 1);

    // 等値面の生成
    for (unsigned int z = 0; z < m_Grid.iNum[2]; ++z)
    {
        for (unsigned int y = 0; y < m_Grid.iNum[1]; ++y)
        {
            for (unsigned int x = 0; x < m_Grid.iNum[0]; ++x)
            {
                // 计算网格中的顶点放置信息表参考索引
                unsigned int tableIndex = 0;
                if (m_ptScalarField[z * slice1 + y * slice0 + x] < m_tIsoLevel)
                    tableIndex |= 1;
                if (m_ptScalarField[z * slice1 + (y + 1) * slice0 + x] < m_tIsoLevel)
                    tableIndex |= 2;
                if (m_ptScalarField[z * slice1 + (y + 1) * slice0 + (x + 1)] < m_tIsoLevel)
                    tableIndex |= 4;
                if (m_ptScalarField[z * slice1 + y * slice0 + (x + 1)] < m_tIsoLevel)
                    tableIndex |= 8;
                if (m_ptScalarField[(z + 1) * slice1 + y * slice0 + x] < m_tIsoLevel)
                    tableIndex |= 16;
                if (m_ptScalarField[(z + 1) * slice1 + (y + 1) * slice0 + x] < m_tIsoLevel)
                    tableIndex |= 32;
                if (m_ptScalarField[(z + 1) * slice1 + (y + 1) * slice0 + (x + 1)] < m_tIsoLevel)
                    tableIndex |= 64;
                if (m_ptScalarField[(z + 1) * slice1 + y * slice0 + (x + 1)] < m_tIsoLevel)
                    tableIndex |= 128;

                if (edgeTable[tableIndex] != 0)
                {
                    // 计算边上的顶点
                    if (edgeTable[tableIndex] & 8)
                    {
                        RxVertexID pt = CalculateIntersection(x, y, z, 3);
                        unsigned int id = GetEdgeID(x, y, z, 3);
                        m_i2pt3idVertices.insert(ID2VertexID::value_type(id, pt));
                    }
                    if (edgeTable[tableIndex] & 1)
                    {
                        RxVertexID pt = CalculateIntersection(x, y, z, 0);
                        unsigned int id = GetEdgeID(x, y, z, 0);
                        m_i2pt3idVertices.insert(ID2VertexID::value_type(id, pt));
                    }
                    if (edgeTable[tableIndex] & 256)
                    {
                        RxVertexID pt = CalculateIntersection(x, y, z, 8);
                        unsigned int id = GetEdgeID(x, y, z, 8);
                        m_i2pt3idVertices.insert(ID2VertexID::value_type(id, pt));
                    }

                    if (x == m_Grid.iNum[0] - 1)
                    {
                        if (edgeTable[tableIndex] & 4)
                        {
                            RxVertexID pt = CalculateIntersection(x, y, z, 2);
                            unsigned int id = GetEdgeID(x, y, z, 2);
                            m_i2pt3idVertices.insert(ID2VertexID::value_type(id, pt));
                        }
                        if (edgeTable[tableIndex] & 2048)
                        {
                            RxVertexID pt = CalculateIntersection(x, y, z, 11);
                            unsigned int id = GetEdgeID(x, y, z, 11);
                            m_i2pt3idVertices.insert(ID2VertexID::value_type(id, pt));
                        }
                    }
                    if (y == m_Grid.iNum[1] - 1)
                    {
                        if (edgeTable[tableIndex] & 2)
                        {
                            RxVertexID pt = CalculateIntersection(x, y, z, 1);
                            unsigned int id = GetEdgeID(x, y, z, 1);
                            m_i2pt3idVertices.insert(ID2VertexID::value_type(id, pt));
                        }
                        if (edgeTable[tableIndex] & 512)
                        {
                            RxVertexID pt = CalculateIntersection(x, y, z, 9);
                            unsigned int id = GetEdgeID(x, y, z, 9);
                            m_i2pt3idVertices.insert(ID2VertexID::value_type(id, pt));
                        }
                    }
                    if (z == m_Grid.iNum[2] - 1)
                    {
                        if (edgeTable[tableIndex] & 16)
                        {
                            RxVertexID pt = CalculateIntersection(x, y, z, 4);
                            unsigned int id = GetEdgeID(x, y, z, 4);
                            m_i2pt3idVertices.insert(ID2VertexID::value_type(id, pt));
                        }
                        if (edgeTable[tableIndex] & 128)
                        {
                            RxVertexID pt = CalculateIntersection(x, y, z, 7);
                            unsigned int id = GetEdgeID(x, y, z, 7);
                            m_i2pt3idVertices.insert(ID2VertexID::value_type(id, pt));
                        }
                    }
                    if ((x == m_Grid.iNum[0] - 1) && (y == m_Grid.iNum[1] - 1))
                        if (edgeTable[tableIndex] & 1024)
                        {
                            RxVertexID pt = CalculateIntersection(x, y, z, 10);
                            unsigned int id = GetEdgeID(x, y, z, 10);
                            m_i2pt3idVertices.insert(ID2VertexID::value_type(id, pt));
                        }
                    if ((x == m_Grid.iNum[0] - 1) && (z == m_Grid.iNum[2] - 1))
                        if (edgeTable[tableIndex] & 64)
                        {
                            RxVertexID pt = CalculateIntersection(x, y, z, 6);
                            unsigned int id = GetEdgeID(x, y, z, 6);
                            m_i2pt3idVertices.insert(ID2VertexID::value_type(id, pt));
                        }
                    if ((y == m_Grid.iNum[1] - 1) && (z == m_Grid.iNum[2] - 1))
                        if (edgeTable[tableIndex] & 32)
                        {
                            RxVertexID pt = CalculateIntersection(x, y, z, 5);
                            unsigned int id = GetEdgeID(x, y, z, 5);
                            m_i2pt3idVertices.insert(ID2VertexID::value_type(id, pt));
                        }

                    // 多边形生成
                    for (unsigned int i = 0; triTable[tableIndex][i] != 255; i += 3)
                    {
                        RxTriangle triangle;
                        unsigned int pointID0, pointID1, pointID2;
                        pointID0 = GetEdgeID(x, y, z, triTable[tableIndex][i]);
                        pointID1 = GetEdgeID(x, y, z, triTable[tableIndex][i + 1]);
                        pointID2 = GetEdgeID(x, y, z, triTable[tableIndex][i + 2]);
                        triangle.pointID[0] = pointID0;
                        triangle.pointID[1] = pointID1;
                        triangle.pointID[2] = pointID2;
                        m_trivecTriangles.push_back(triangle);
                    }
                }
            }
        }
    }

    RenameVerticesAndTriangles(vrts, m_nVertices, tris, m_nTriangles);
    CalculateNormals(vrts, m_nVertices, tris, m_nTriangles, nrms, m_nNormals);

    m_bValidSurface = true;
}

/*
 *获得边缘ID
 * @param [in] nX，nY，nZ网格位置
 * @param [in] nEdgeNo边数
 * @return边缘ID
 */
unsigned int rxMCMesh::GetEdgeID(unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo)
{
    switch (nEdgeNo)
    {
    case 0:
        return GetVertexID(nX, nY, nZ) + 1;
    case 1:
        return GetVertexID(nX, nY + 1, nZ);
    case 2:
        return GetVertexID(nX + 1, nY, nZ) + 1;
    case 3:
        return GetVertexID(nX, nY, nZ);
    case 4:
        return GetVertexID(nX, nY, nZ + 1) + 1;
    case 5:
        return GetVertexID(nX, nY + 1, nZ + 1);
    case 6:
        return GetVertexID(nX + 1, nY, nZ + 1) + 1;
    case 7:
        return GetVertexID(nX, nY, nZ + 1);
    case 8:
        return GetVertexID(nX, nY, nZ) + 2;
    case 9:
        return GetVertexID(nX, nY + 1, nZ) + 2;
    case 10:
        return GetVertexID(nX + 1, nY + 1, nZ) + 2;
    case 11:
        return GetVertexID(nX + 1, nY, nZ) + 2;
    default:
        // Invalid edge no.
        return -1;
    }
}

/*！
 *获取顶点ID
 * @param [in] nX，nY，nZ网格位置
 * @return顶点ID
 */
unsigned int rxMCMesh::GetVertexID(unsigned int nX, unsigned int nY, unsigned int nZ)
{
    return 3 * (nZ * (m_Grid.iNum[1] + 1) * (m_Grid.iNum[0] + 1) + nY * (m_Grid.iNum[0] + 1) + nX);
}

/*！
 * 通过插值计算边缘上的等值点（从样本量）
 * @param [in] nX，nY，nZ网格位置
 * @param [in] nEdgeNo边数
 * @return网格顶点信息
 */
RxVertexID rxMCMesh::CalculateIntersection(unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo)
{
    double x1, y1, z1, x2, y2, z2;
    unsigned int v1x = nX, v1y = nY, v1z = nZ;
    unsigned int v2x = nX, v2y = nY, v2z = nZ;

    switch (nEdgeNo)
    {
    case 0:
        v2y += 1;
        break;
    case 1:
        v1y += 1;
        v2x += 1;
        v2y += 1;
        break;
    case 2:
        v1x += 1;
        v1y += 1;
        v2x += 1;
        break;
    case 3:
        v1x += 1;
        break;
    case 4:
        v1z += 1;
        v2y += 1;
        v2z += 1;
        break;
    case 5:
        v1y += 1;
        v1z += 1;
        v2x += 1;
        v2y += 1;
        v2z += 1;
        break;
    case 6:
        v1x += 1;
        v1y += 1;
        v1z += 1;
        v2x += 1;
        v2z += 1;
        break;
    case 7:
        v1x += 1;
        v1z += 1;
        v2z += 1;
        break;
    case 8:
        v2z += 1;
        break;
    case 9:
        v1y += 1;
        v2y += 1;
        v2z += 1;
        break;
    case 10:
        v1x += 1;
        v1y += 1;
        v2x += 1;
        v2y += 1;
        v2z += 1;
        break;
    case 11:
        v1x += 1;
        v2x += 1;
        v2z += 1;
        break;
    }

    // 获取边的两点坐标
    x1 = m_Grid.fMin[0] + v1x * m_Grid.fWidth[0];
    y1 = m_Grid.fMin[1] + v1y * m_Grid.fWidth[1];
    z1 = m_Grid.fMin[2] + v1z * m_Grid.fWidth[2];
    x2 = m_Grid.fMin[0] + v2x * m_Grid.fWidth[0];
    y2 = m_Grid.fMin[1] + v2y * m_Grid.fWidth[1];
    z2 = m_Grid.fMin[2] + v2z * m_Grid.fWidth[2];

    unsigned int slice0 = (m_Grid.iNum[0] + 1);
    unsigned int slice1 = slice0 * (m_Grid.iNum[1] + 1);
    float val1 = m_ptScalarField[v1z * slice1 + v1y * slice0 + v1x];
    float val2 = m_ptScalarField[v2z * slice1 + v2y * slice0 + v2x];
    RxVertexID intersection = Interpolate(x1, y1, z1, x2, y2, z2, val1, val2);

    return intersection;
}

RxVertexID rxMCMesh::Interpolate(double fX1, double fY1, double fZ1, double fX2, double fY2, double fZ2, float tVal1, float tVal2)
{
    RxVertexID res;
    float a = fabs(tVal1 - m_tIsoLevel);
    float b = fabs(tVal2 - m_tIsoLevel);
    float t = a / (a + b);
    res.x = fX1 * t + fX2 * (1 - t);
    res.y = fY1 * t + fY2 * (1 - t);
    res.z = fZ1 * t + fZ2 * (1 - t);
    return res;
}

/*！
 *网格顶点，以输出格式存储的几何信息
 * @param [out] vrts顶点坐标
 * @param [out] nvrts顶点数
 * @param [out] tris三角形多边形几何信息
 * @param [out] ntris三角形多边形的数量
 */
void rxMCMesh::RenameVerticesAndTriangles(vector<glm::vec3> &vrts, unsigned int &nvrts, vector<int> &tris, unsigned int &ntris)
{
    unsigned int nextID = 0;
    ID2VertexID::iterator mapIterator = m_i2pt3idVertices.begin();
    RxTriangleVector::iterator vecIterator = m_trivecTriangles.begin();

    // 刷新点
    while (mapIterator != m_i2pt3idVertices.end())
    {
        (*mapIterator).second.newID = nextID;
        nextID++;
        mapIterator++;
    }

    // 刷新三角面.
    while (vecIterator != m_trivecTriangles.end())
    {
        for (unsigned int i = 0; i < 3; i++)
        {
            unsigned int newID = m_i2pt3idVertices[(*vecIterator).pointID[i]].newID;
            (*vecIterator).pointID[i] = newID;
        }
        vecIterator++;
    }

    // 将所有顶点和三角形复制到两个数组中，以便可以有效地访问它们。
    // 复制点
    mapIterator = m_i2pt3idVertices.begin();
    nvrts = (int)m_i2pt3idVertices.size();
    vrts.resize(nvrts);
    for (unsigned int i = 0; i < nvrts; i++, mapIterator++)
    {
        vrts[i][0] = (*mapIterator).second.x;
        vrts[i][1] = (*mapIterator).second.y;
        vrts[i][2] = (*mapIterator).second.z;
    }
    // 复制制作三角形的顶点索引。
    vecIterator = m_trivecTriangles.begin();
    ntris = (int)m_trivecTriangles.size();
    tris.resize(ntris * 3);
    for (unsigned int i = 0; i < ntris; i++, vecIterator++)
    {
        tris[3 * i + 0] = (*vecIterator).pointID[0];
        tris[3 * i + 1] = (*vecIterator).pointID[1];
        tris[3 * i + 2] = (*vecIterator).pointID[2];
    }

    // 释放空间
    m_i2pt3idVertices.clear();
    m_trivecTriangles.clear();
}

/*！
 *顶点法线计算
 * @param [in] vrts顶点坐标
 * @param [in] nvrts顶点数
 * @param [in] tris三角形多边形几何信息
 * @param [in] ntris三角形多边形的数量
 * @param [out] nrms法线
 * @param [out] nnrms法线数（=顶点数）
 */
void rxMCMesh::CalculateNormals(const vector<glm::vec3> &vrts, unsigned int nvrts, const vector<int> &tris, unsigned int ntris, vector<glm::vec3> &nrms, unsigned int &nnrms)
{
    nnrms = nvrts;
    nrms.resize(nnrms);

    // 初始化法线.
    for (unsigned int i = 0; i < nnrms; i++)
    {
        nrms[i][0] = 0;
        nrms[i][1] = 0;
        nrms[i][2] = 0;
    }

    // 计算法线.
    for (unsigned int i = 0; i < ntris; i++)
    {
        glm::vec3 vec1, vec2, normal;
        unsigned int id0, id1, id2;
        id0 = tris[3 * i + 0];
        id1 = tris[3 * i + 1];
        id2 = tris[3 * i + 2];

        vec1 = vrts[id1] - vrts[id0];
        vec2 = vrts[id2] - vrts[id0];
        normal = cross(vec1, vec2);

        nrms[id0] += normal;
        nrms[id1] += normal;
        nrms[id2] += normal;
    }

    // 单位化法线.
    for (unsigned int i = 0; i < nnrms; i++)
    {
        normalize(nrms[i]);
    }
}