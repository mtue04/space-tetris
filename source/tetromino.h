#ifndef TETROMINO_H
#define TETROMINO_H

#include "definition.h"

class Block {
private:
    int type;					// 7 types of block
    int rotation;				// 4 types of rotation : 0deg, 90deg, 180deg, 270deg
    int row, col;               // row, col pos
public:
    Block(int type, int rotation);
    Block(const Block& block);

    int get_type();
    int get_type(int row_offset, int col_offset);
    int get_rotation();
    void set_type(int type);
    void set_rotation(int rotation);

    int get_row();
    int get_col();
    void set_row(int row);
    void set_col(int col);

    int get_init_row_offset();
    int get_init_col_offset();

    void swap(Block& a, Block& b);
};

#endif // !TETROMINO_H