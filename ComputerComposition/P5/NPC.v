`timescale 1ns / 1ps

`include "define.v"
module NPC(
		input [31:0] iPC,
		input [31:0] iinstr,
		input [31:0] irs,
		input [2:0] iNPC_sel,
		output [31:0] oNPC,
		output [31:0] oPC8
    );

	reg [31:0] rNPC;
	initial rNPC = 32'h3004;
	
	always@(*) begin
		if(iNPC_sel == `NPC_SEQUENCE) rNPC = iPC + 32'h4;
		else if(iNPC_sel == `NPC_BRANCH) rNPC = iPC /*+ 32'h4 */+ {{14{iinstr[15]}}, iinstr[15:0], 2'b00};
		else if(iNPC_sel == `NPC_JUMP) rNPC = {iPC[31:28], iinstr[25:0], 2'b00};
		else if(iNPC_sel == `NPC_JUMPREG) rNPC = irs;
		else rNPC = iPC + 32'h4; // default
	end
	
	assign oNPC = rNPC;
	assign oPC8 = iPC + 32'h8;

endmodule
