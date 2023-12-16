#include "board.h"

Board::Board() {
	for (int row = 0; row < playfield_height; row++)
		for (int col = 0; col < playfield_width; col++)
			board_state[row][col] = BlockStatus::empty_block;
}

int Board::get_tetromino(int row, int col) {
	return (int)(board_state[row][col]) - 1;
}

bool Board::is_empty_block(int row, int col) {
	return (board_state[row][col] == BlockStatus::empty_block);
}

bool Board::is_legal_position(Block block) {
	for (int row = block.get_row(); row < (block.get_row() + matrix_blocks); row++) {
		for (int col = block.get_col(); col < (block.get_col() + matrix_blocks); col++) {
			// Outside playfield limits
			if (row < 0 || row >(playfield_height - 1) || col < 0 || col >(playfield_width - 1))
				if (block.get_type(row - block.get_row(), col - block.get_col()) != 0)
					return false;

			// Collide filled block
			if (row >= 0)
				if (block.get_type(row - block.get_row(), col - block.get_col()) != 0 && !is_empty_block(row, col))
					return false;
		}
	}
	return true;
}

void Board::store_block(Block block) {
	for (int row = block.get_row(); row < (block.get_row() + matrix_blocks); row++)
		for (int col = block.get_col(); col < (block.get_col() + matrix_blocks); col++)
			if (block.get_type(row - block.get_row(), col - block.get_col()) != 0)
				board_state[row][col] = static_cast<BlockStatus>(1 + block.get_type());
	blocks.push_back(block);
}

void Board::delete_row(int row) {
	for (int r = row; r > 0; r--)
		for (int c = 0; c < playfield_width; c++)
			board_state[r][c] = board_state[r - 1][c];
}

int Board::delete_full_rows() {
	int num_deleted_rows = 0;
	for (int row = 0; row < playfield_height; row++) {
		bool full_row = true;

		for (int col = 0; col < playfield_width; col++)
			if (is_empty_block(row, col))
				full_row = false;

		if (full_row) {
			num_deleted_rows++;
			delete_row(row);
		}
	}
	return num_deleted_rows;
}

bool Board::is_game_over() {
	for (int col = 0; col < playfield_width; col++)
		if (board_state[0][col] != BlockStatus::empty_block || board_state[1][col] != BlockStatus::empty_block)
			return true;
	return false;
}