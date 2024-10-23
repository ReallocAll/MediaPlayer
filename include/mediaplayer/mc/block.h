#pragma once
#include <libcutils/libcutils.h>
#include <mediaplayer/mc/symbols.h>
#include "position.h"

struct block;
struct block_source;

enum direction {
    DIRECTION_NEG_Y,
    DIRECTION_POS_Y,
    DIRECTION_NEG_Z,
    DIRECTION_POS_Z,
    DIRECTION_NEG_X,
    DIRECTION_POS_X,
};

struct block *get_block(struct block_source *bs, const struct block_pos *bl_pos);
enum direction get_block_face_direction(struct block *bl);
uintptr_t get_block_state_anvil_damage(struct block *bl, uintptr_t a2);
