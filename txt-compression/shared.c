#include "shared.h"
#include <stdlib.h>

void free_huffman_tree(struct huffman_node *root) {
  if (root->ascii_code) {
    free(root);
    return;
  }

  if (root->left) {
    free_huffman_tree(root->left);
  }

  if (root->right) {
    free_huffman_tree(root->right);
  }

  free(root);
}