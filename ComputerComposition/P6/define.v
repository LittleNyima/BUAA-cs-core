`timescale 1ns / 1ps

`include "decode.v"
`ifndef MACRO_DEFINITION
`define MACRO_DEFINITION

`define ALU_OP_ADD   4'b0000
`define ALU_OP_SUB   4'b0001
`define ALU_OP_SLL   4'b0010
`define ALU_OP_SRL   4'b0011
`define ALU_OP_SRA   4'b0100
`define ALU_OP_AND   4'b0101
`define ALU_OP_OR    4'b0110
`define ALU_OP_XOR   4'b0111
`define ALU_OP_NOR   4'b1000
`define ALU_OP_SLT   4'b1001
`define ALU_OP_SLTU  4'b1010
`define ALU_OP_SLLV  4'b1011
`define ALU_OP_SRLV  4'b1100
`define ALU_OP_SRAV  4'b1101

`define ALU_SRC_GRF  1'b0
`define ALU_SRC_EXT  1'b1

`define CMP_EQUAL    3'b000
`define CMP_NEQUAL   3'b001
`define CMP_RSLTZ    3'b010
`define CMP_RSLEZ    3'b011
`define CMP_RSGTZ    3'b100
`define CMP_RSGEZ    3'b101

`define DM_NONE      4'b0000
`define DM_SB        4'b0001
`define DM_SH        4'b0010
`define DM_SW        4'b0011
`define DM_LB        4'b0100
`define DM_LBU       4'b0101
`define DM_LH        4'b0110
`define DM_LHU       4'b0111
`define DM_LW        4'b1000

`define EXT_UNSIGNED 3'b000
`define EXT_SIGNED   3'b001
`define EXT_HIGHHALF 3'b010
`define EXT_SHIFTAMT 3'b011

`define GRF_WD_RD    2'b00
`define GRF_WD_RT    2'b01
`define GRF_WD_1F    2'b10

`define GRF_WS_ALU   2'b00
`define GRF_WS_DM    2'b01
`define GRF_WS_PC8   2'b10
`define GRF_WS_MD    2'b11

`define MD_OP_NONE   4'b0000
`define MD_OP_MULT   4'b0001
`define MD_OP_MULTU  4'b0010
`define MD_OP_DIV    4'b0100
`define MD_OP_DIVU   4'b1000

`define MD_SEL_HI    1'b0
`define MD_SEL_LO    1'b1

`define MF_NONE2D    3'b000
`define MF_M2D       3'b001
`define MF_MPC2D     3'b010
`define MF_MMD2D     3'b011
`define MF_W2D       3'b100

`define MF_NONE2E    3'b000
`define MF_M2E       3'b001
`define MF_MPC2E     3'b010
`define MF_MMD2E     3'b011
`define MF_W2E       3'b100

`define MF_NONE2M    1'b0
`define MF_W2M       1'b1

`define NPC_SEQUENCE 3'b000
`define NPC_BRANCH   3'b001
`define NPC_JUMP     3'b010
`define NPC_JUMPREG  3'b011 

`define TNEW_ALU     3'b000
`define TNEW_DM      3'b001
`define TNEW_PC      3'b010
`define TNEW_MD      3'b011
`define TNEW_NULL    3'b111

`endif
