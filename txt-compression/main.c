#include "debug.h"
#include "encoder.h"
#include "shared.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

int read_n_bit(int n, int value) {
  int mask = 1 << (n - 1);
  return value & mask;
}

void decode_byte(struct decoder_state *ds, uint8_t byte, int padding_bits) {
  for (int bit_num = 8; bit_num > padding_bits; bit_num--) {
    if (ds->curr->ascii_code) {
      ds->output[ds->insert_idx] = ds->curr->ascii_code;
      ds->insert_idx++;
      ds->curr = ds->root;
    }

    int bit = read_n_bit(bit_num, byte);

    if (bit == 0) {
      ds->curr = ds->curr->left;
    } else {
      ds->curr = ds->curr->right;
    }
  }
}

// this is temp solution, it doesnt make sense to hold lets say 2gb of text in memory
// chunk should get written into file straight away
char *decode_text(struct huffman_node *root_node, uint8_t *encoded_text, struct encoded_size es) {
  struct decoder_state ds = {root_node, root_node, 0, malloc(CHUNK_SIZE)};

  for (size_t i = 0; i < es.bytes; i++) {
    uint8_t byte = encoded_text[i];
    decode_byte(&ds, byte, 0);
  }

  uint8_t last_byte = encoded_text[es.bytes - 1];
  decode_byte(&ds, last_byte, es.padding_bits);
  ds.output[ds.insert_idx] = 0;

  return ds.output;
}

int main(void) {
  FILE *fptr = fopen("raw.txt", "rb");
  uint8_t *encoded_text = encode_ascii_text(fptr);

  // char *decoded_text = decode_text(root_node, encoded_text, encoded_size);
  // printf("%s\n", decoded_text);

  // struct file_header fh = create_file_header(unique_chars_count, encoded_size, huffman_table);
  // printf("ts:%i pb:%i\n", fh.table_size, fh.padding_bits);

  // free(fh.huffman_table);
  // free(decoded_text);
  // free_huffman_tree(root_node);
  free(encoded_text);
  fclose(fptr);

  return 0;
}