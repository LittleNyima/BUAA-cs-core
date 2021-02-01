`timescale 1ns / 1ps

`ifndef INSTRUCTION_LIST
`define INSTRUCTION_LIST

`define INSTRUCTIONS reg lb, lbu, lh, lhu, lw, sb, sh, sw;\
							reg add, addu, sub, subu, sll, srl, sra, sllv, srlv, srav;\
							reg and_, or_, xor_, nor_;\
							reg mult, multu, div, divu;\
							reg addi, addiu, andi, ori, xori, lui;\
							reg slt, slti, sltiu, sltu;\
							reg beq, bne, blez, bgtz, bltz, bgez;\
							reg j, jal, jalr, jr;\
							reg mfhi, mflo, mthi, mtlo;\
							reg nop;\
							
`define SPLIT wire [5:0] opcode, func;\
				  wire [4:0] rs_, rt_, rd_;\
				  assign opcode = iinstr[31:26];\
				  assign rs_ = iinstr[25:21];\
				  assign rt_ = iinstr[20:16];\
				  assign rd_ = iinstr[15:11];\
				  assign func = iinstr[5:0];\

`define DECODE lb    = (opcode == 6'b100000);\
					lbu   = (opcode == 6'b100100);\
					lh    = (opcode == 6'b100001);\
					lhu   = (opcode == 6'b100101);\
					lw    = (opcode == 6'b100011);\
					sb    = (opcode == 6'b101000);\
					sh    = (opcode == 6'b101001);\
					sw    = (opcode == 6'b101011);\
					add   = (opcode == 6'b000000) && (func == 6'b100000);\
					addu  = (opcode == 6'b000000) && (func == 6'b100001);\
					sub   = (opcode == 6'b000000) && (func == 6'b100010);\
					subu  = (opcode == 6'b000000) && (func == 6'b100011);\
					sll   = (opcode == 6'b000000) && (func == 6'b000000) && (rs_ == 5'b00000) && ~(rd_ == 5'b00000);\
					srl   = (opcode == 6'b000000) && (func == 6'b000010) && (rs_ == 5'b00000);\
					sra   = (opcode == 6'b000000) && (func == 6'b000011) && (rs_ == 5'b00000);\
					sllv  = (opcode == 6'b000000) && (func == 6'b000100);\
					srlv  = (opcode == 6'b000000) && (func == 6'b000110);\
					srav  = (opcode == 6'b000000) && (func == 6'b000111);\
					and_  = (opcode == 6'b000000) && (func == 6'b100100);\
					or_   = (opcode == 6'b000000) && (func == 6'b100101);\
					xor_  = (opcode == 6'b000000) && (func == 6'b100110);\
					nor_  = (opcode == 6'b000000) && (func == 6'b100111);\
					mult  = (opcode == 6'b000000) && (func == 6'b011000);\
					multu = (opcode == 6'b000000) && (func == 6'b011001);\
					div   = (opcode == 6'b000000) && (func == 6'b011010);\
					divu  = (opcode == 6'b000000) && (func == 6'b011011);\
					addi  = (opcode == 6'b001000);\
					addiu = (opcode == 6'b001001);\
					andi  = (opcode == 6'b001100);\
					ori   = (opcode == 6'b001101);\
					xori  = (opcode == 6'b001110);\
					lui   = (opcode == 6'b001111);\
					slt   = (opcode == 6'b000000) && (func == 6'b101010);\
					slti  = (opcode == 6'b001010);\
					sltiu = (opcode == 6'b001011);\
					sltu  = (opcode == 6'b000000) && (func == 6'b101011);\
					beq   = (opcode == 6'b000100);\
					bne   = (opcode == 6'b000101);\
					blez  = (opcode == 6'b000110) && (rt_ == 5'b00000);\
					bgtz  = (opcode == 6'b000111) && (rt_ == 5'b00000);\
					bltz  = (opcode == 6'b000001) && (rt_ == 5'b00000);\
					bgez  = (opcode == 6'b000001) && (rt_ == 5'b00001);\
					j     = (opcode == 6'b000010);\
					jal   = (opcode == 6'b000011);\
					jalr  = (opcode == 6'b000000) && (func == 6'b001001);\
					jr    = (opcode == 6'b000000) && (func == 6'b001000);\
					mfhi  = (opcode == 6'b000000) && (func == 6'b010000);\
					mflo  = (opcode == 6'b000000) && (func == 6'b010010);\
					mthi  = (opcode == 6'b000000) && (func == 6'b010001);\
					mtlo  = (opcode == 6'b000000) && (func == 6'b010011);\
					nop   = (iinstr == 32'h00000000);\

`endif
