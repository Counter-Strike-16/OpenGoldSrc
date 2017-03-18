



void Z_ClearZone (memzone_t *zone, int size)
{
	
// set the entire zone to one free block

	zone->blocklist.next = zone->blocklist.prev = block =
		(memblock_t *)( (byte *)zone + sizeof(memzone_t) );
	zone->blocklist.tag = 1;	// in use block
	zone->blocklist.id = 0;
	zone->blocklist.size = 0;
	zone->rover = block;
	
}



void Z_Free (void *ptr)
{
	block = (memblock_t *) ( (byte *)ptr - sizeof(memblock_t));
	
	
	if (!other->tag)
	{	// merge with previous free block
		other->size += block->size;
		other->next = block->next;
		other->next->prev = other;
		if (block == mainzone->rover)
			mainzone->rover = other;
		block = other;
	}
	
	if (!other->tag)
	{	
		block->next->prev = block;
	}
}

//============================================================================

void *Hunk_AllocName (int size, char *name)
{	
	size = sizeof(hunk_t) + ((size+15)&~15);
	
	if (hunk_size - hunk_low_used - hunk_high_used < size)
//		Sys_Error ("Hunk_Alloc: failed on %i bytes",size);
#ifdef _WIN32
	  	Sys_Error ("Not enough RAM allocated.  Try starting using \"-heapsize 16000\" on the QuakeWorld command line.");
#else
	  	Sys_Error ("Not enough RAM allocated.  Try starting using \"-mem 16\" on the QuakeWorld command line.");
#endif
	
	hunk_low_used += size;

	memset (h, 0, size);
	
	h->size = size;
	Q_strncpy (h->name, name, 8);
	
}

typedef struct cache_system_s
{
	int						size;		// including this header
	cache_user_t			*user;
	char					name[16];
	struct cache_system_s	*prev, *next;
	struct cache_system_s	*lru_prev, *lru_next;	// for LRU flushing	
} cache_system_t;

cache_system_t *Cache_TryAlloc (int size, qboolean nobottom);

/*
===========
Cache_Move
===========
*/
void Cache_Move ( cache_system_t *c)
{
	cache_system_t		*new;

// we are clearing up space at the bottom, so only allocate it late
	new = Cache_TryAlloc (c->size, true);
	if (new)
	{
//		Con_Printf ("cache_move ok\n");

		Q_memcpy ( new+1, c+1, c->size - sizeof(cache_system_t) );
		new->user = c->user;
		Q_memcpy (new->name, c->name, sizeof(new->name));
		Cache_Free (c->user);
		new->user->data = (void *)(new+1);
	}
	else
	{
//		Con_Printf ("cache_move failed\n");

		Cache_Free (c->user);		// tough luck...
	}
}

/*
============
Cache_FreeHigh

Throw things out until the hunk can be expanded to the given point
============
*/
void Cache_FreeHigh (int new_high_hunk)
{
	cache_system_t	*c, *prev;
	
	prev = NULL;
	while (1)
	{
		c = cache_head.prev;
		if (c == &cache_head)
			return;		// nothing in cache at all
		if ( (byte *)c + c->size <= hunk_base + hunk_size - new_high_hunk)
			return;		// there is space to grow the hunk
		if (c == prev)
			Cache_Free (c->user);	// didn't move out of the way
		else
		{
			Cache_Move (c);	// try to move it
			prev = c;
		}
	}
}

void Cache_UnlinkLRU (cache_system_t *cs)
{
	if (!cs->lru_next || !cs->lru_prev)
		Sys_Error ("Cache_UnlinkLRU: NULL link");

	cs->lru_next->lru_prev = cs->lru_prev;
	cs->lru_prev->lru_next = cs->lru_next;
	
	cs->lru_prev = cs->lru_next = NULL;
}

void Cache_MakeLRU (cache_system_t *cs)
{
	if (cs->lru_next || cs->lru_prev)
		Sys_Error ("Cache_MakeLRU: active link");

	cache_head.lru_next->lru_prev = cs;
	cs->lru_next = cache_head.lru_next;
	cs->lru_prev = &cache_head;
	cache_head.lru_next = cs;
}

/*
============
Cache_TryAlloc

Looks for a free block of memory between the high and low hunk marks
Size should already include the header and padding
============
*/
cache_system_t *Cache_TryAlloc (int size, qboolean nobottom)
{
	cache_system_t	*cs, *new;
	
// is the cache completely empty?

	if (!nobottom && cache_head.prev == &cache_head)
	{
		if (hunk_size - hunk_high_used - hunk_low_used < size)
			Sys_Error ("Cache_TryAlloc: %i is greater then free hunk", size);

		new = (cache_system_t *) (hunk_base + hunk_low_used);
		memset (new, 0, sizeof(*new));
		new->size = size;

		cache_head.prev = cache_head.next = new;
		new->prev = new->next = &cache_head;
		
		Cache_MakeLRU (new);
		return new;
	}
	
// search from the bottom up for space

	new = (cache_system_t *) (hunk_base + hunk_low_used);
	cs = cache_head.next;
	
	do
	{
		if (!nobottom || cs != cache_head.next)
		{
			if ( (byte *)cs - (byte *)new >= size)
			{	// found space
				memset (new, 0, sizeof(*new));
				new->size = size;
				
				new->next = cs;
				new->prev = cs->prev;
				cs->prev->next = new;
				cs->prev = new;
				
				Cache_MakeLRU (new);
	
				return new;
			}
		}

	// continue looking		
		new = (cache_system_t *)((byte *)cs + cs->size);
		cs = cs->next;

	} while (cs != &cache_head);
	
// try to allocate one at the very end
	if ( hunk_base + hunk_size - hunk_high_used - (byte *)new >= size)
	{
		memset (new, 0, sizeof(*new));
		new->size = size;
		
		new->next = &cache_head;
		new->prev = cache_head.prev;
		cache_head.prev->next = new;
		cache_head.prev = new;
		
		Cache_MakeLRU (new);

		return new;
	}
	
	return NULL;		// couldn't allocate
}

/*
============
Cache_Flush

Throw everything out, so new data will be demand cached
============
*/
void Cache_Flush (void)
{
	while (cache_head.next != &cache_head)
		Cache_Free ( cache_head.next->user );	// reclaim the space
}


/*
============
Cache_Print

============
*/
void Cache_Print (void)
{
	cache_system_t	*cd;

	for (cd = cache_head.next ; cd != &cache_head ; cd = cd->next)
	{
		Con_Printf ("%8i : %s\n", cd->size, cd->name);
	}
}

/*
============
Cache_Report

============
*/
void Cache_Report (void)
{
	Con_DPrintf ("%4.1f megabyte data cache\n", (hunk_size - hunk_high_used - hunk_low_used) / (float)(1024*1024) );
}

/*
============
Cache_Compact

============
*/
void Cache_Compact (void)
{
}

/*
============
Cache_Init

============
*/
void Cache_Init (void)
{
	cache_head.next = cache_head.prev = &cache_head;
	cache_head.lru_next = cache_head.lru_prev = &cache_head;

	Cmd_AddCommand ("flush", Cache_Flush);
}

void Cache_Free (cache_user_t *c)
{
	cs = ((cache_system_t *)c->data) - 1;

	cs->next = cs->prev = NULL;

	c->data = NULL;
}



/*
==============
Cache_Check
==============
*/
void *Cache_Check (cache_user_t *c)
{
	cache_system_t	*cs;

	if (!c->data)
		return NULL;

	cs = ((cache_system_t *)c->data) - 1;

// move to head of LRU
	Cache_UnlinkLRU (cs);
	Cache_MakeLRU (cs);
	
	return c->data;
}

void *Cache_Alloc (cache_user_t *c, int size, char *name)
{
	size = (size + sizeof(cache_system_t) + 15) & ~15;


	while (1)
	{
		cs = Cache_TryAlloc (size, false);
		if (cs)
		{
			strncpy (cs->name, name, sizeof(cs->name)-1);
			c->data = (void *)(cs+1);
		}
													
		
	} 
}

void Memory_Init (void *buf, int size)
{
	int zonesize = DYNAMIC_SIZE;

	hunk_base = buf;
	
	mainzone = Hunk_AllocName ( zonesize, "zone" );
}