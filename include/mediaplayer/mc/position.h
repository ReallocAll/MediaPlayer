#pragma once

struct vec3 {
	float x, y, z;
};

struct actor_pos {
    struct vec3 curr;    // +0
    struct vec3 prev;    // +12
    struct vec3 delta;   // +24
};

struct block_pos {
	int x, y, z;
};
