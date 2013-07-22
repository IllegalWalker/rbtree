/*
 * rbtree.h
 *
 *  Created on: 2013-6-25
 *      Author: Wilman Lau
 */

#ifndef RBTREE_H_
#define RBTREE_H_

// Data type to be put in rbtree node.
typedef void * RB_DATA_T;
typedef int (*RB_COMPARE_FUNC)(RB_DATA_T data1, RB_DATA_T data2);

struct rb_node
{
	unsigned long  rb_parent_color;
#define	RB_RED		0
#define	RB_BLACK	1
	struct rb_node *rb_right;
	struct rb_node *rb_left;
	RB_DATA_T data;		// user defined data field
} __attribute__((aligned(sizeof(long))));
/* The alignment might seem pointless, but allegedly CRIS needs it */

typedef struct rb_root
{
	struct rb_node *rb_node;

	RB_COMPARE_FUNC compare;
}RB_ROOT_T;

/* Helper Macros */
#define rb_parent(r)   ((struct rb_node *)((r)->rb_parent_color & ~3))
#define rb_color(r)   ((r)->rb_parent_color & 1)
#define rb_is_red(r)   (!rb_color(r))
#define rb_is_black(r) rb_color(r)
/* Use following macros like functions */
#define rb_set_red(r)  do { (r)->rb_parent_color &= ~1; } while (0)
#define rb_set_black(r)  do { (r)->rb_parent_color |= 1; } while (0)
#define rb_set_data(r, d)  do { (r)->data = d; } while (0)

static inline void rb_set_parent(struct rb_node *rb, struct rb_node *p)
{
	rb->rb_parent_color = (rb->rb_parent_color & 3) | (unsigned long)p;
}
static inline void rb_set_color(struct rb_node *rb, int color)
{
	rb->rb_parent_color = (rb->rb_parent_color & ~1) | color;
}

/* Rbtree node/root creation and free function. */
extern struct rb_node * rb_create_node(const RB_DATA_T data);
extern void rb_free_node(struct rb_node *node);
extern struct rb_root * rb_create_tree(const RB_COMPARE_FUNC cmp);
extern void rb_free_tree(struct rb_root *root);

/* Rbtree operations function. */
extern struct rb_node *rb_search(struct rb_root * root, const RB_DATA_T data);
extern bool rb_insert_node(struct rb_node * node, struct rb_root * root);
extern bool rb_insert_data(RB_DATA_T data, struct rb_root * root);
extern void rb_detach_node(struct rb_node *node, struct rb_root *root);
extern void rb_delete_node(struct rb_node *node, struct rb_root *root);
extern bool rb_delete_data(const RB_DATA_T data, struct rb_root *root);

/* Find logical next and previous nodes in a tree */
extern struct rb_node *rb_next(const struct rb_node *);
extern struct rb_node *rb_prev(const struct rb_node *);
extern struct rb_node *rb_first(const struct rb_root *);
extern struct rb_node *rb_last(const struct rb_root *);

#endif /* RBTREE_H_ */
