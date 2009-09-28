#ifndef ZZGO_UCT_TREE_H
#define ZZGO_UCT_TREE_H

#include <stdbool.h>
#include "move.h"

struct board;
struct uct_policy;

/*
 *            +------+
 *            | node |
 *            +------+
 *          / <- parent
 * +------+   v- sibling +------+
 * | node | ------------ | node |
 * +------+              +------+
 *    | <- children          |
 * +------+   +------+   +------+   +------+
 * | node | - | node |   | node | - | node |
 * +------+   +------+   +------+   +------+
 */

struct move_stats {
	int playouts; // # of playouts coming through this node
	int wins; // # of wins coming through this node
	float value; // wins/playouts
};

struct tree_node {
	hash_t hash;
	struct tree_node *parent, *sibling, *children;

	/*** From here on, struct is saved/loaded from opening book */

	int depth; // just for statistics

	coord_t coord;

	struct move_stats u;
	struct move_stats prior;
	/* XXX: Should be way for policies to add their own stats */
	struct move_stats amaf;
#define NODE_HINT_NOAMAF 0x80
	int hints;
};

struct tree {
	struct board *board;
	struct tree_node *root;
	struct board_symmetry root_symmetry;
	enum stone root_color;

	// Statistics
	int max_depth;
};

struct tree *tree_init(struct board *board, enum stone color);
void tree_done(struct tree *tree);
void tree_dump(struct tree *tree, int thres);
void tree_save(struct tree *tree, struct board *b, int thres);
void tree_load(struct tree *tree, struct board *b, enum stone color);
struct tree *tree_copy(struct tree *tree);
void tree_merge(struct tree *dest, struct tree *src);

void tree_expand_node(struct tree *tree, struct tree_node *node, struct board *b, enum stone color, int radar, struct uct_policy *policy, int parity);
void tree_delete_node(struct tree *tree, struct tree_node *node);
void tree_promote_node(struct tree *tree, struct tree_node *node);
bool tree_promote_at(struct tree *tree, struct board *b, coord_t c);
bool tree_leaf_node(struct tree_node *node);
void tree_update_node_value(struct tree_node *node);

#define tree_node_get_value(tree, node, type, parity) \
	(parity > 0 ? node->type.value : 1 - node->type.value)
#define tree_node_get_wins(tree, node, type, parity) \
	(parity > 0 ? node->type.wins : node->type.playouts - node->type.wins)

#endif
