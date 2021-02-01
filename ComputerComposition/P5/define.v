`timescale 1ns / 1ps

`ifndef MACRO_DEFINITION
`define MACRO_DEFINITION

`define ALU_OP_ADD 3'b000
`define ALU_OP_SUB 3'b001
`define ALU_OP_OR  3'b010

`define ALU_SRC_GRF 1'b0
`define ALU_SRC_EXT 1'b1

`define EXT_UNSIGNED 3'b000
`define EXT_SIGNED   3'b001
`define EXT_HIGHHALF 3'b010

`define GRF_WD_RT 2'b00
`define GRF_WD_RD 2'b01
`define GRF_WD_1F 2'b10

`define GRF_WS_ALU 2'b00
`define GRF_WS_DM  2'b01
`define GRF_WS_PC8 2'b10

`define MF_M2D    2'b00
`define MF_W2D    2'b01
`define MF_MPC2D  2'b10
`define MF_NONE2D 2'b11
`define MF_M2E    2'b00
`define MF_W2E    2'b01
`define MF_MPC2E  2'b10
`define MF_NONE2E 2'b11
`define MF_W2M    1'b0
`define MF_NONE2M 1'b1

`define NPC_SEQUENCE 3'b000
`define NPC_BRANCH   3'b001
`define NPC_JUMP     3'b010
`define NPC_JUMPREG  3'b011

`define TNEW_ALU  3'b000
`define TNEW_DM   3'b001
`define TNEW_PC   3'b010
`define TNEW_NULL 3'b111

`define INSTR_DEF reg addu, subu, ori, lw, sw, beq, lui, j, jal, jr, nop;

`define DECODE addu = (opcode == 6'b000000) && (func == 6'b100001);\
					subu = (opcode == 6'b000000) && (func == 6'b100011);\
					ori = opcode == 6'b001101;\
					lw = opcode == 6'b100011;\
					sw = opcode == 6'b101011;\
					beq = opcode == 6'b000100;\
					lui = opcode == 6'b001111;\
					j = opcode == 6'b000010;\
					jal = opcode == 6'b000011;\
					jr = (opcode == 6'b000000) && (func == 6'b001000);\
					nop = (opcode == 6'b000000) && (func == 6'b000000);\

`endif
