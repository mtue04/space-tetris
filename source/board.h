#ifndef BOARD_H
#define BOARD_H

#include "header.h"

class Board {
	enum BlockStatus : int { empty_block, i_block, o_block, t_block, s_block, z_block, j_block, l_block };
private:
	BlockStatus board_state[playfield_height][playfield_width];
	vector<Block> blocks;
public:
	Board();

	int get_tetromino(int row, int col);
	bool is_empty_block(int row, int col);
	bool is_legal_position(Block block);
	void store_block(Block block);

	void delete_row(int row);
	int delete_full_rows();
	bool is_game_over();
};

#endif // !BOARD_H