/*
 ============================================================================
 Name        : rbtree.c
 Author      : Wilman Lau
 Version     :
 Copyright   : This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation;

 Description : red black tree implemented by C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "rbtree.h"


struct rb_node * rb_create_node(const RB_DATA_T data)
{
	struct rb_node *node = malloc(sizeof(struct rb_node));
	if (node)
	{
		memset(node, 0, sizeof(struct rb_node));
		node->data = data;
		return node;
	}
	else
	{
		fprintf(stderr, "FATAL ERROR: Insufficient memory, malloc rbtree node failed.\n");
		exit(EXIT_FAILURE);
	}
}

void rb_free_node(struct rb_node *node)
{
	if (node)
	{
		if (node->data)
			free(node->data);
		free(node);
	}
}

struct rb_root * rb_create_tree(const RB_COMPARE_FUNC cmp)
{
	struct rb_root *root = malloc(sizeof(struct rb_root));
	if (root)
	{
		memset(root, 0, sizeof(struct rb_root));
		root->compare = cmp;
		return root;
	}
	else
	{
		printf("ERROR: malloc rbtree root failed.\n");
		exit(-1);
	}
}

/* Free sub tree, auxiliary function for rb_free_tree().*/
void _rb_free_subtree(struct rb_node *node)
{
	if (!node)
		return;

	if(node->rb_left)
	{
		_rb_free_subtree(node->rb_left);
		node->rb_left = NULL;
	}

	if(node->rb_right)
	{
		_rb_free_subtree(node->rb_right);
		node->rb_right = NULL;
	}

	if (!node->rb_left && !node->rb_right)
		rb_free_node(node);
}

void rb_free_tree(struct rb_root *root)
{
	if (!root)
		return;

	// Free all nodes of tree.
	_rb_free_subtree(root->rb_node);

	free(root);
}

/*Rotate left code of a binary sort tree.*/
static void __rb_rotate_left(struct rb_node *node, struct rb_root *root)
{
	struct rb_node *right = node->rb_right;
	struct rb_node *parent = rb_parent(node);

	if ((node->rb_right = right->rb_left))
		rb_set_parent(right->rb_left, node);
	right->rb_left = node;

	rb_set_parent(right, parent);

	if (parent)
	{
		if (node == parent->rb_left)
			parent->rb_left = right;
		else
			parent->rb_right = right;
	}
	else
		root->rb_node = right;
	rb_set_parent(node, right);
}

/*Rotate right code of a binary sort tree.*/
static void __rb_rotate_right(struct rb_node *node, struct rb_root *root)
{
	struct rb_node *left = node->rb_left;
	struct rb_node *parent = rb_parent(node);

	if ((node->rb_left = left->rb_right))
		rb_set_parent(left->rb_right, node);
	left->rb_right = node;

	rb_set_parent(left, parent);

	if (parent)
	{
		if (node == parent->rb_right)
			parent->rb_right = left;
		else
			parent->rb_left = left;
	}
	else
		root->rb_node = left;
	rb_set_parent(node, left);
}

/*Search tree for node whose data field equals data argument.*/
struct rb_node *rb_search(struct rb_root * root, const RB_DATA_T data)
{
	if (!root || !data || !root->rb_node)
		return NULL;

	struct rb_node *node = root->rb_node;
	RB_COMPARE_FUNC compare = root->compare;

	while(node)
	{
		if (compare(data, node->data) < 0)
			node = node->rb_left;
		else if (compare(data, node->data) > 0)
			node = node->rb_right;
		else
			return node;
	}
	return NULL;
}

/* Return parent of node whose data equal data argument.*/
struct rb_node *_rb_search_parent(struct rb_root * root, const RB_DATA_T data)
{
	if (!root || !data)
		return NULL;

	struct rb_node *parent = NULL;
	struct rb_node *node = root->rb_node;
	RB_COMPARE_FUNC compare = root->compare;

	while(node)
	{
		if (compare(data, node->data) < 0)
		{
			parent = node;
			node = node->rb_left;
		}
		else if (compare(data, node->data) > 0)
		{
			parent = node;
			node = node->rb_right;
		}
		else
			return parent;
	}

	return parent;
}

/*Function to fix up a corrupted rbtree after inserting a node.*/
static void __rb_insert_fixup(struct rb_node *node, struct rb_root *root)
{
	struct rb_node *parent, *gparent;

	while ((parent = rb_parent(node)) && rb_is_red(parent))
	{
		gparent = rb_parent(parent);

		if (parent == gparent->rb_left)
		{
			{
				register struct rb_node *uncle = gparent->rb_right;
				// Case1: node's uncle is red.
				if (uncle && rb_is_red(uncle))
				{
					rb_set_black(uncle);
					rb_set_black(parent);
					rb_set_red(gparent);
					node = gparent;
					continue;
				}
			}

			// Case2: node's uncle is black(Null as a black node, as <introduction to algorithm> did>)
			//        and node's uncle is right child of grandparent.
			if (parent->rb_right == node)
			{
				register struct rb_node *tmp;
				__rb_rotate_left(parent, root);
				tmp = parent;
				parent = node;
				node = tmp;
			}

			rb_set_black(parent);
			rb_set_red(gparent);
			__rb_rotate_right(gparent, root);
		} else {
			{
				register struct rb_node *uncle = gparent->rb_left;
				// Case1: node's uncle is red.
				if (uncle && rb_is_red(uncle))
				{
					rb_set_black(uncle);
					rb_set_black(parent);
					rb_set_red(gparent);
					node = gparent;
					continue;
				}
			}
			// Case3: node's uncle is black and node's uncle is left child of grandparent.
			if (parent->rb_left == node)
			{
				register struct rb_node *tmp;
				__rb_rotate_right(parent, root);
				tmp = parent;
				parent = node;
				node = tmp;
			}

			rb_set_black(parent);
			rb_set_red(gparent);
			__rb_rotate_left(gparent, root);
		}
	}

	rb_set_black(root->rb_node);
}

/* Insert a node, if there is a node already in the tree and
 * its data equals data of node to be inserted, return false. */
bool rb_insert_node(struct rb_node * node, struct rb_root * root)
{
	if (!node || !root)
		return false;

	// Empty tree.
	if (!root->rb_node)
	{
		root->rb_node = node;
		rb_set_parent(node, NULL);
		rb_set_black(node);
		return true;
	}

	struct rb_node *parent = _rb_search_parent(root, node->data);
	rb_set_red(node);
	rb_set_parent(node, parent);
	if (root->compare(node->data, parent->data)<0 && parent->rb_left == NULL)
		parent->rb_left = node;
	else if (root->compare(node->data, parent->data)>0 && parent->rb_right == NULL)
		parent->rb_right = node;
	else
		return false;

	__rb_insert_fixup(node, root);

	return true;
}

/* Argument data should only be freed implicitly when tree is freed. */
bool rb_insert_data(RB_DATA_T data, struct rb_root * root)
{
	struct rb_node *new_node = rb_create_node(data);
	if (new_node)
		return rb_insert_node(new_node, root);
	else
		return false;
}

/* Function to fix up a corrupted rbtree after deleting a node. */
static void __rb_delete_node_fixup(struct rb_node *node, struct rb_node *parent,
			     struct rb_root *root)
{
	struct rb_node *brother;

	while ((!node || rb_is_black(node)) && node != root->rb_node)
	{
		if (parent->rb_left == node)
		{
			brother = parent->rb_right;
			// Case1: node's brother is red.
			// Case1 will be converted to Case2 , Case3 or Case4 after this operation.
			if (rb_is_red(brother))
			{
				rb_set_black(brother);
				rb_set_red(parent);
				__rb_rotate_left(parent, root);
				brother = parent->rb_right;
			}
			// When got here brother is definitely black.

			// Case2: node's brother is black, and brother's children are both black.
			if ((!brother->rb_left || rb_is_black(brother->rb_left)) &&
			    (!brother->rb_right || rb_is_black(brother->rb_right)))
			{
				rb_set_red(brother);
				node = parent;
				parent = rb_parent(node);
			}
			else
			{
				// Case3: brother is black, and brother's left child is red, bother's right child is black.
				// Case3 will be converted to Case4 after this operation.
				if (!brother->rb_right || rb_is_black(brother->rb_right))
				{
					rb_set_black(brother->rb_left);
					rb_set_red(brother);
					__rb_rotate_right(brother, root);
					brother = parent->rb_right;
				}
				// Case4: brother is black and brother's right child is red.
				rb_set_color(brother, rb_color(parent));
				rb_set_black(parent);
				rb_set_black(brother->rb_right);
				__rb_rotate_left(parent, root);
				node = root->rb_node;
				break;
			}
		}
		else
		{
			brother = parent->rb_left;
			// Case1: node's brother is red.
			if (rb_is_red(brother))
			{
				rb_set_black(brother);
				rb_set_red(parent);
				__rb_rotate_right(parent, root);
				brother = parent->rb_left;
			}
			// Case2: node's brother is black, and brother's children are both black.
			if ((!brother->rb_left || rb_is_black(brother->rb_left)) &&
			    (!brother->rb_right || rb_is_black(brother->rb_right)))
			{
				rb_set_red(brother);
				node = parent;
				parent = rb_parent(node);
			}
			else
			{
				// Case3: brother is black, and brother's left child is red, bother's right child is black.
				if (!brother->rb_left || rb_is_black(brother->rb_left))
				{
					rb_set_black(brother->rb_right);
					rb_set_red(brother);
					__rb_rotate_left(brother, root);
					brother = parent->rb_left;
				}
				// Case4: brother is black and brother's right child is red.
				rb_set_color(brother, rb_color(parent));
				rb_set_black(parent);
				rb_set_black(brother->rb_left);
				__rb_rotate_right(parent, root);
				node = root->rb_node;
				break;
			}
		}
	}
	if (node)
		rb_set_black(node);
}

/* Detach a node from rbtree with its memory not freed. */
void rb_detach_node(struct rb_node *node, struct rb_root *root)
{
	if (node == NULL || root == NULL)
		return;

	struct rb_node *child, *parent;
	int color;

	if (!node->rb_left)
		child = node->rb_right;
	else if (!node->rb_right)
		child = node->rb_left;
	else
	{
		struct rb_node *old = node, *left;

		node = node->rb_right;
		while ((left = node->rb_left) != NULL)
			node = left;

		if (rb_parent(old)) {
			if (rb_parent(old)->rb_left == old)
				rb_parent(old)->rb_left = node;
			else
				rb_parent(old)->rb_right = node;
		} else
			root->rb_node = node;

		child = node->rb_right;
		parent = rb_parent(node);
		color = rb_color(node);

		if (parent == old) {
			parent = node;
		} else {
			if (child)
				rb_set_parent(child, parent);
			parent->rb_left = child;

			node->rb_right = old->rb_right;
			rb_set_parent(old->rb_right, node);
		}

		node->rb_parent_color = old->rb_parent_color;
		node->rb_left = old->rb_left;
		rb_set_parent(old->rb_left, node);

		goto color;
	}

	parent = rb_parent(node);
	color = rb_color(node);

	if (child)
		rb_set_parent(child, parent);
	if (parent)
	{
		if (parent->rb_left == node)
			parent->rb_left = child;
		else
			parent->rb_right = child;
	}
	else
		root->rb_node = child;

 color:
	if (color == RB_BLACK)
		__rb_delete_node_fixup(child, parent, root);
}

/* Delete a node from rbtree, with its memory freed. */
void rb_delete_node(struct rb_node *node, struct rb_root *root)
{
	rb_detach_node(node, root);
	rb_free_node(node);
}

/* Delete a node, whose data equal argument data, from rbtree with its memory freed. */
bool rb_delete_data(const RB_DATA_T data, struct rb_root *root)
{
	struct rb_node *node = rb_search(root, data);
	if (node)
	{
		rb_delete_node(node, root);
		return true;
	}
	return false;
}

/*
 * This function returns the first node (in sort order) of the tree.
 */
struct rb_node *rb_first(const struct rb_root *root)
{
	struct rb_node	*n;

	n = root->rb_node;
	if (!n)
		return NULL;
	while (n->rb_left)
		n = n->rb_left;
	return n;
}

struct rb_node *rb_last(const struct rb_root *root)
{
	struct rb_node	*n;

	n = root->rb_node;
	if (!n)
		return NULL;
	while (n->rb_right)
		n = n->rb_right;
	return n;
}

struct rb_node *rb_next(const struct rb_node *node)
{
	struct rb_node *parent;

	if (rb_parent(node) == node)
		return NULL;

	/* If we have a right-hand child, go down and then left as far
	   as we can. */
	if (node->rb_right) {
		node = node->rb_right;
		while (node->rb_left)
			node=node->rb_left;
		return (struct rb_node *)node;
	}

	/* No right-hand children.  Everything down and left is
	   smaller than us, so any 'next' node must be in the general
	   direction of our parent. Go up the tree; any time the
	   ancestor is a right-hand child of its parent, keep going
	   up. First time it's a left-hand child of its parent, said
	   parent is our 'next' node. */
	while ((parent = rb_parent(node)) && node == parent->rb_right)
		node = parent;

	return parent;
}

struct rb_node *rb_prev(const struct rb_node *node)
{
	struct rb_node *parent;

	if (rb_parent(node) == node)
		return NULL;

	/* If we have a left-hand child, go down and then right as far
	   as we can. */
	if (node->rb_left) {
		node = node->rb_left;
		while (node->rb_right)
			node=node->rb_right;
		return (struct rb_node *)node;
	}

	/* No left-hand children. Go up till we find an ancestor which
	   is a right-hand child of its parent */
	while ((parent = rb_parent(node)) && node == parent->rb_left)
		node = parent;

	return parent;
}
