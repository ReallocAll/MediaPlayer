#include <mediaplayer/mc/item_stack.h>


struct item_stack *get_carried_item(struct actor *actor)
{
    return TLCALL("?getCarriedItem@Actor@@UEBAAEBVItemStack@@XZ",
                    struct item_stack *(*)(struct actor *actor),
                    actor);
}
