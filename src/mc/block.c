#include <mediaplayer/mc/block.h>


struct block *get_block(struct block_source *bs, const struct block_pos *bl_pos)
{
	return
		SYMCALL(S_BlockSource__getBlock,
			struct block * (*)(struct block_source *, const struct block_pos *),
			bs, bl_pos);
}


enum direction get_block_face_direction(struct block *bl)
{
	char buf[24];
	*(uintptr_t *)&buf[8] = 18;
    return (enum direction)get_block_state_anvil_damage(bl, (uintptr_t)&buf);
}


uintptr_t get_block_state_anvil_damage(struct block *bl, uintptr_t a2) 
{
    return
		SYMCALL(S_Block__getState__AnvilDamage,
			unsigned char (*)(struct block *this, uintptr_t a2),
			bl, a2);
}
